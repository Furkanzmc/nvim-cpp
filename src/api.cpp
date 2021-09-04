#include "nvim/api.h"

#include "nvim/connection.h"
#include "nvim/exceptions.h"
#include "nvim/request.h"
#include "nvim/response.h"
#include "nvim/private/event_store.h"
#include "nvim/generated/nvim_api_generated.h"

#include <msgpack/pack.hpp>
#include <msgpack/unpack.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include <tuple>
#include <iostream>
#include <algorithm>

using namespace boost;

namespace {
std::uint64_t s_last_msg_id{ 0 };

void start_loop_internal(const nvim::types::connection& connection,
                         nvim::types::loop_callback callback,
                         nvim::types::error_callback err_callback);

[[nodiscard]] std::string object_type_string(msgpack::type::object_type type)
  NVIM_NOEXCEPT
{
    static const std::map<msgpack::type::object_type, std::string> type_map{
        { msgpack::type::object_type::NIL, "NIL" },
        { msgpack::type::object_type::BOOLEAN, "BOOLEAN" },
        { msgpack::type::object_type::POSITIVE_INTEGER, "POSITIVE_INTEGER" },
        { msgpack::type::object_type::NEGATIVE_INTEGER, "NEGATIVE_INTEGER" },
        { msgpack::type::object_type::FLOAT32, "FLOAT32" },
        { msgpack::type::object_type::FLOAT64, "FLOAT64" },
        { msgpack::type::object_type::FLOAT, "FLOAT" },
        { msgpack::type::object_type::STR, "STR" },
        { msgpack::type::object_type::BIN, "BIN" },
        { msgpack::type::object_type::ARRAY, "ARRAY" },
        { msgpack::type::object_type::MAP, "MAP" },
        { msgpack::type::object_type::EXT, "EXT" }
    };

    assert(type_map.find(type) != type_map.end());
    return type_map.at(type);
}

nvim::types::response parse_response(const msgpack::object& obj) NVIM_NOEXCEPT
{
    std::tuple<nvim::types::integer_t,
               nvim::types::integer_t,
               nvim::types::object_t,
               nvim::types::object_t>
      dst;

    obj.convert(dst);

    if (nvim::api::detail::has_error(obj)) {
        const msgpack::object* error = obj.via.array.ptr[2].via.array.ptr;
        return nvim::types::response{ std::get<1>(dst),
                                      static_cast<nvim::types::message_type>(
                                        std::get<0>(dst)),
                                      {},
                                      { error[1].as<nvim::types::string_t>(),
                                        error[0].as<nvim::types::integer_t>() },
                                      "" };
    }

    return nvim::types::response{ std::get<1>(dst),
                                  static_cast<nvim::types::message_type>(
                                    std::get<0>(dst)),
                                  { std::get<3>(dst) },
                                  {},
                                  "" };
}

void parse_notification(const msgpack::object& obj)
{
    std::tuple<nvim::types::integer_t,
               nvim::types::object_t,
               nvim::types::object_t>
      dst;

    obj.convert(dst);

    const auto res = nvim::types::response{
        -1,
        static_cast<nvim::types::message_type>(std::get<0>(dst)),
        { std::get<2>(dst) },
        {},
        std::get<1>(dst).as_string()
    };

    auto events = nvim::api::detail::parse_events(res);
    nvim::api::detail::push_events(std::move(events));
}

void on_read_ready(const nvim::types::connection& connection,
                   const boost::system::error_code& error_code,
                   nvim::types::loop_callback callback,
                   nvim::types::error_callback err_callback) NVIM_NOEXCEPT
{
    assert(callback);
    assert(err_callback);

    if (error_code) {
        err_callback(error_code);
        return;
    }

    nvim::api::detail::get_response_async(
      connection,
      [&connection,
       callback = std::move(callback),
       err_callback =
         std::move(err_callback)](const nvim::types::response& response) {
          if (response.msg_type == nvim::types::message_type::response &&
              nvim::api::detail::has_response_callback(response.msg_id)) {
              const auto response_callback =
                nvim::api::detail::get_response_callback(response.msg_id);
              assert(response_callback);
              response_callback(response);
          }

          callback();
          start_loop_internal(
            connection, std::move(callback), std::move(err_callback));
      });
}

void start_loop_internal(const nvim::types::connection& connection,
                         nvim::types::loop_callback callback,
                         nvim::types::error_callback err_callback)
{
    assert(callback);
    assert(err_callback);

    nvim::api::detail::wait_read_async(
      connection,
      [&connection,
       callback = std::move(callback),
       err_callback = std::move(err_callback)](const auto& error_code) {
          on_read_ready(connection, error_code, callback, err_callback);
      });
}
}

namespace nvim {
namespace api {

namespace detail {
boost::system::error_code wait_read(const nvim::types::connection& connection)
  NVIM_NOEXCEPT
{
    boost::system::error_code error_code;
    connection.socket->wait(boost::asio::ip::tcp::socket::wait_read,
                            error_code);
    return error_code;
}

void wait_read_async(const nvim::types::connection& connection,
                     nvim::types::wait_callback callback) NVIM_NOEXCEPT
{
    connection.socket->async_wait(boost::asio::ip::tcp::socket::wait_read,
                                  std::move(callback));
}

void start_loop(const nvim::types::connection& connection,
                nvim::types::loop_callback callback,
                nvim::types::error_callback err_callback) NVIM_NOEXCEPT
{
    assert(callback);
    assert(err_callback);
    auto guard{ asio::make_work_guard(*connection.io_context.get()) };

    start_loop_internal(
      connection, std::move(callback), std::move(err_callback));

    connection.io_context->restart();
    connection.io_context->run();
}

void stop_loop(const nvim::types::connection& connection) NVIM_NOEXCEPT
{
    connection.io_context->stop();
}

nvim::types::request setup_request(const std::string& method,
                                   std::uint32_t arg_count) NVIM_NOEXCEPT
{
    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> pk{ &sbuffer };
    pk.pack_array(4);

    // type
    pk.pack_int(0);

    const auto msg_id = ++s_last_msg_id;
    // msg_id
    pk.pack_int(msg_id);

    // method
    pk.pack_bin(method.size());
    pk.pack_bin_body(method.c_str(), method.size());

    // arguments
    pk.pack_array(arg_count);

    return { std::move(sbuffer), arg_count, msg_id };
}

std::size_t write(const nvim::types::connection& cnn,
                  const nvim::types::request& request,
                  nvim::types::milliseconds ms)
{
    assert(cnn.socket);
    assert(cnn.io_context);
    assert(cnn.socket->is_open());
    assert(request.arg_count == request.args.count);

    system::error_code error;
    bool handler_called{ false };
    std::size_t bytes_transferred{ 0 };
    asio::async_write(
      *cnn.socket.get(),
      asio::buffer(request.buffer.data(), request.buffer.size()),
      [&error, &handler_called, &bytes_transferred](
        const system::error_code& err, std::size_t bt) {
          error = err;
          bytes_transferred = bt;
          handler_called = true;
      });

    cnn.io_context->restart();
    cnn.io_context->run_for(std::chrono::milliseconds{ ms });
    if (error) {
        throw nvim::exceptions::request_error{ error };
    }
    else if (!handler_called) {
        throw nvim::exceptions::timeout_error{};
    }

    return bytes_transferred;
}

void write_async(const nvim::types::connection& cnn,
                 const nvim::types::request& request) NVIM_NOEXCEPT
{
    assert(cnn.socket);
    assert(cnn.io_context);
    assert(cnn.socket->is_open());
    assert(request.arg_count == request.args.count);
    assert(request.on_bytes_written);

    asio::async_write(
      *cnn.socket.get(),
      asio::buffer(request.buffer.data(), request.buffer.size()),
      request.on_bytes_written);

    cnn.io_context->restart();
}

std::uint64_t read(const nvim::types::connection& cnn,
                   nvim::types::in_buffer* buf,
                   std::uint64_t length,
                   nvim::types::milliseconds ms)
{
    assert(cnn.socket);
    assert(cnn.io_context);
    assert(cnn.socket->is_open());

    system::error_code error;
    std::size_t bytes_transferred{ 0 };
    bool handler_called{ false };
    auto handler = [&error, &bytes_transferred, &handler_called](
                     const system::error_code& err, std::size_t bt) {
        error = err;
        bytes_transferred = bt;
        handler_called = true;
    };

    asio::async_read(*cnn.socket.get(),
                     asio::buffer(buf, length),
                     asio::transfer_at_least(1),
                     std::move(handler));

    cnn.io_context->restart();
    cnn.io_context->run_for(std::chrono::milliseconds{ ms });
    if (error) {
        throw nvim::exceptions::request_error{ error };
    }
    else if (!handler_called) {
        throw nvim::exceptions::timeout_error{};
    }

    return bytes_transferred;
}

void read_async(const nvim::types::connection& cnn,
                nvim::types::in_buffer* buf,
                std::uint64_t length,
                nvim::types::read_callback callback) NVIM_NOEXCEPT
{
    assert(cnn.socket);
    assert(cnn.io_context);
    assert(cnn.socket->is_open());

    asio::async_read(*cnn.socket.get(),
                     asio::buffer(buf, length),
                     asio::transfer_at_least(1),
                     std::move(callback));

    cnn.io_context->restart();
}

void validate_msgpack_object(const msgpack::object& object)
{
    using object_type = msgpack::type::object_type;

    if (object.type != object_type::ARRAY) {
        throw nvim::exceptions::bad_response{
            std::string{ "Response type must be an array but received: " } +
            object_type_string(object.via.array.ptr[0].type)
        };
    }

    if (object.via.array.size < 3 || object.via.array.size > 4) {
        throw nvim::exceptions::bad_response{
            std::string{
              "Response array lenght must be 3 or for but received: " } +
            std::to_string(object.via.array.size)
        };
    }

    if (object.via.array.ptr[0].type != object_type::POSITIVE_INTEGER) {
        throw nvim::exceptions::bad_response{
            std::string{ "msg_type must be an integer but received: " } +
            object_type_string(object.via.array.ptr[0].type)
        };
    }

    const auto ptr = object.via.array.ptr;
    const auto type = static_cast<nvim::types::message_type>(ptr[0].via.u64);

    switch (type) {
        case nvim::types::message_type::request:
            if (ptr[1].type != object_type::POSITIVE_INTEGER) {
                throw nvim::exceptions::bad_response{
                    std::string{
                      "msg_id must be a positive integer but received: " } +
                    object_type_string(ptr[1].type)
                };
            }

            if (ptr[2].type != object_type::BIN &&
                ptr[2].type != object_type::STR) {
                throw nvim::exceptions::bad_response{
                    std::string{ "method must be a string but received: " } +
                    object_type_string(ptr[2].type)
                };
            }

            if (ptr[3].type != object_type::ARRAY) {
                throw nvim::exceptions::bad_response{
                    std::string{ "Expected an array but received: " } +
                    object_type_string(ptr[2].type)
                };
            }

            break;
        case nvim::types::message_type::response:
            if (ptr[1].type != object_type::POSITIVE_INTEGER) {
                throw nvim::exceptions::bad_response{
                    std::string{
                      "msg_id must be a positive integer but received: " } +
                    object_type_string(ptr[1].type)
                };
            }
            break;
        case nvim::types::message_type::notification:
            break;
        default:
            throw nvim::exceptions::bad_response{
                std::string{ "Unsupported type: " } +
                std::to_string(static_cast<std::uint8_t>(type))
            };
    }
}

bool has_error(const msgpack::object& object) NVIM_NOEXCEPT
{
    return object.via.array.ptr[2].type != msgpack::type::object_type::NIL;
}

nvim::types::response get_response(const nvim::types::connection& connection,
                                   nvim::types::milliseconds ms)
{
    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(32 * 1024ul);

    const auto bytes_read = nvim::api::detail::read(
      connection, unpacker.buffer(), unpacker.buffer_capacity(), ms);
    if (bytes_read == 0) {
        throw nvim::exceptions::bad_response{ "No bytes were read." };
    }

    unpacker.buffer_consumed(bytes_read);

    msgpack::unpacked result;
    unpacker.next(result);
    const msgpack::object& obj = result.get();

    validate_msgpack_object(obj);

    const auto message_type =
      static_cast<nvim::types::message_type>(obj.via.array.ptr[0].via.u64);

    if (message_type == nvim::types::message_type::response) {
        return parse_response(obj);
    }

    assert(message_type == nvim::types::message_type::notification);

    parse_notification(obj);
    return nvim::types::response{
        {}, nvim::types::message_type::notification, {}, {}, ""
    };
}

void get_response_async(const nvim::types::connection& connection,
                        nvim::types::response_callback callback) NVIM_NOEXCEPT
{
    std::shared_ptr<msgpack::unpacker> unpacker{ new msgpack::unpacker{} };
    unpacker->reserve_buffer(32 * 1024ul);

    nvim::api::detail::read_async(
      connection,
      unpacker->buffer(),
      unpacker->buffer_capacity(),
      [unpacker = std::move(unpacker), callback = std::move(callback)](
        const boost::system::error_code& error, std::size_t bytes_read) {
          if (error) {
              assert(false);
          }
          else {
              unpacker->buffer_consumed(bytes_read);
          }

          msgpack::unpacked result;
          unpacker->next(result);
          const msgpack::object& obj = result.get();

          validate_msgpack_object(obj);

          const auto message_type = static_cast<nvim::types::message_type>(
            obj.via.array.ptr[0].via.u64);

          if (message_type == nvim::types::message_type::response) {
              callback(parse_response(obj));
          }
          else {
              assert(message_type == nvim::types::message_type::notification);

              parse_notification(obj);
              callback(nvim::types::response{
                {}, nvim::types::message_type::notification, {}, {}, "" });
          }
      });
}
}
}
}
