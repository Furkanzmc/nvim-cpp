#include "nvim/api.h"
#include "nvim/connection.h"
#include "nvim/exceptions.h"
#include "nvim/request.h"
#include "nvim/response.h"

#include "test_utils.h"

#include <msgpack/unpack.hpp>

#include <catch2/catch_test_macros.hpp>

SCENARIO("A function with return value is called.", "[nvim::api]")
{
    nvim_process np;

    nvim::types::connection connection{ std::move(
      nvim::connect_tcp("127.0.0.1", 6667)) };

    REQUIRE(connection.socket->is_open());
    GIVEN("A valid connection")
    {
        WHEN("A function is called with wrong arguments.")
        {

            nvim::types::request request{ nvim::api::detail::setup_request(
              "nvim_buf_get_changedtick", 0) };

            CHECK_NOTHROW(nvim::api::detail::write(connection, request, 1));

            THEN("has_error returns true.")
            {
                msgpack::unpacker unpacker;
                unpacker.reserve_buffer(32 * 1024ul);

                const auto bytes_read =
                  nvim::api::detail::read(connection,
                                          unpacker.buffer(),
                                          unpacker.buffer_capacity(),
                                          1000);
                REQUIRE(bytes_read > 0);

                unpacker.buffer_consumed(bytes_read);

                msgpack::unpacked result;
                while (unpacker.next(result)) {
                    auto obj = result.get();

                    CHECK(nvim::api::detail::has_error(obj));
                }
            }
        }

        WHEN("A function is called with correct arguments.")
        {

            nvim::types::request request{ nvim::api::detail::setup_request(
              "nvim_buf_get_changedtick", 1) };
            request.args << 0;

            CHECK_NOTHROW(nvim::api::detail::write(connection, request, 1));

            THEN("has_error returns false.")
            {
                msgpack::unpacker unpacker;
                unpacker.reserve_buffer(32 * 1024ul);

                const auto bytes_read =
                  nvim::api::detail::read(connection,
                                          unpacker.buffer(),
                                          unpacker.buffer_capacity(),
                                          1000);
                REQUIRE(bytes_read > 0);

                unpacker.buffer_consumed(bytes_read);

                msgpack::unpacked result;
                while (unpacker.next(result)) {
                    auto obj = result.get();

                    CHECK(nvim::api::detail::has_error(obj) == false);
                }
            }
        }
    }
}

SCENARIO("A function with return value is called and response read.",
         "[nvim::api]")
{
    nvim_process np;

    nvim::types::connection connection{ std::move(
      nvim::connect_tcp("127.0.0.1", 6667)) };

    REQUIRE(connection.socket->is_open());

    WHEN("A function with EXT type is called.")
    {
        nvim::types::request request{ nvim::api::detail::setup_request(
          "nvim_get_current_buf", 0) };

        CHECK_NOTHROW(nvim::api::detail::write(connection, request, 1));

        THEN("Returns a valid response.")
        {
            const nvim::types::response response =
              nvim::api::detail::get_response(connection);

            CHECK(response.error.code == -1);
            CHECK(response.msg_id == request.msg_id);
            CHECK(response.msg_type == nvim::types::message_type::response);
            CHECK(response.value->is_ext());
            CHECK(response.value == 1);
            const nvim::types::integer_t bufnr = response.value;
            CHECK(bufnr == 1);
        }
    }

    WHEN("A function with int type is called.")
    {
        nvim::types::request request{ nvim::api::detail::setup_request(
          "nvim_buf_get_changedtick", 1) };
        request.args << 1;

        CHECK_NOTHROW(nvim::api::detail::write(connection, request, 1));

        THEN("Returns a valid response.")
        {
            const nvim::types::response response =
              nvim::api::detail::get_response(connection);

            CHECK(response.error.code == -1);
            CHECK(response.msg_id == request.msg_id);
            CHECK(response.msg_type == nvim::types::message_type::response);
            CHECK(response.value->is_uint64_t());
            CHECK(static_cast<nvim::types::uinteger_t>(response.value) > 1);
        }
    }
}

SCENARIO("An async function with return value is called and response read.",
         "[nvim::api]")
{
    nvim_process np;

    nvim::types::connection connection{ std::move(
      nvim::connect_tcp("127.0.0.1", 6667)) };

    REQUIRE(connection.socket->is_open());

    WHEN("A function with int type is called.")
    {
        nvim::types::request request{ nvim::api::detail::setup_request(
          "nvim_buf_get_changedtick", 1) };
        request.args << 1;

        bool handler_called{ false };
        request.on_bytes_written =
          [&request, &handler_called](const boost::system::error_code& error,
                                      std::size_t bt) {
              REQUIRE(bt > 0);
              REQUIRE_FALSE(error.failed());
              handler_called = true;
          };

        nvim::api::detail::write_async(connection, request);

        connection.io_context->run_for(std::chrono::milliseconds(500));

        REQUIRE(handler_called);
        handler_called = false;

        THEN("Returns a valid response.")
        {
            nvim::api::detail::get_response_async(
              connection,
              [&handler_called,
               &request](const nvim::types::response& response) {
                  handler_called = true;

                  CHECK(response.error.code == -1);
                  CHECK(response.msg_id == request.msg_id);
                  CHECK(response.msg_type ==
                        nvim::types::message_type::response);
                  CHECK(response.value->is_uint64_t());
                  CHECK(static_cast<nvim::types::uinteger_t>(response.value) >
                        1);
              });

            connection.io_context->run_for(std::chrono::milliseconds(500));
            REQUIRE(handler_called);
        }
    }
}

SCENARIO("A request is created and arguments need to be added",
         "[nvim::types::request]")
{
    nvim::types::request r{ {}, 3 };
    r.args << 2;
    r.args << 2.f;
    r.args << 3.;
}
