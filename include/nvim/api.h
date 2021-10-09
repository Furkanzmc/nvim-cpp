#ifndef NVIM_CPP_API_H
#define NVIM_CPP_API_H

#include "nvim/types.h"
#include "nvim/response.h"
#include "nvim/defs.h"

namespace nvim {

namespace types {
struct connection;
struct request;
struct response;
}

namespace api::detail {

boost::system::error_code wait_read(const nvim::types::connection& connection)
  NVIM_NOEXCEPT;

void wait_read_async(const nvim::types::connection& connection,
                     nvim::types::wait_callback callback) NVIM_NOEXCEPT;

void start_loop(const nvim::types::connection& connection,
                nvim::types::loop_callback callback,
                nvim::types::error_callback err_callback) NVIM_NOEXCEPT;

void stop_loop(const nvim::types::connection& connection) NVIM_NOEXCEPT;

[[nodiscard]] nvim::types::request setup_request(const std::string& method,
                                                 std::uint32_t arg_count)
  NVIM_NOEXCEPT;

[[maybe_unused]] std::size_t write(const nvim::types::connection& cnn,
                                   const nvim::types::request& request,
                                   nvim::types::milliseconds ms);

void write_async(const nvim::types::connection& cnn,
                 const nvim::types::request& request) NVIM_NOEXCEPT;

[[nodiscard]] std::uint64_t read(const nvim::types::connection& cnn,
                                 nvim::types::in_buffer* buf,
                                 std::uint64_t length,
                                 nvim::types::milliseconds ms);

void read_async(const nvim::types::connection& cnn,
                nvim::types::in_buffer* buf,
                std::uint64_t length,
                nvim::types::read_callback callback) NVIM_NOEXCEPT;

void validate_msgpack_object(const msgpack::object& object);

[[nodiscard]] bool has_error(const msgpack::object& object) NVIM_NOEXCEPT;

[[nodiscard]] nvim::types::response get_response(
  const nvim::types::connection& connection,
  nvim::types::milliseconds ms = 1000);

void get_response_async(const nvim::types::connection& connection,
                        nvim::types::response_callback callback) NVIM_NOEXCEPT;
}
}

#endif
