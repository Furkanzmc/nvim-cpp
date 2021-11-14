#ifndef NVIM_CPP_API_H
#define NVIM_CPP_API_H

#include "nvim/types.h"
#include "nvim/response.h"
#include "nvim/defs.h"
#include "nvim/export.h"

namespace nvim {

namespace types {
struct connection;
struct request;
struct response;
}

namespace api::detail {

NVIM_CPP_EXPORT boost::system::error_code wait_read(
  const nvim::types::connection& connection) NVIM_NOEXCEPT;

NVIM_CPP_EXPORT void wait_read_async(const nvim::types::connection& connection,
                                     nvim::types::wait_callback callback)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT void start_loop(const nvim::types::connection& connection,
                                nvim::types::loop_callback callback,
                                nvim::types::error_callback err_callback)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT void stop_loop(const nvim::types::connection& connection)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[nodiscard]] nvim::types::request setup_request(
  const std::string& method,
  std::uint32_t arg_count) NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] std::size_t write(
  const nvim::types::connection& cnn,
  const nvim::types::request& request,
  nvim::types::milliseconds ms);

NVIM_CPP_EXPORT void write_async(const nvim::types::connection& cnn,
                                 const nvim::types::request& request)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[nodiscard]] std::uint64_t read(
  const nvim::types::connection& cnn,
  nvim::types::in_buffer* buf,
  std::uint64_t length,
  nvim::types::milliseconds ms);

NVIM_CPP_EXPORT void read_async(const nvim::types::connection& cnn,
                                nvim::types::in_buffer* buf,
                                std::uint64_t length,
                                nvim::types::read_callback callback)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT void validate_msgpack_object(const msgpack::object& object);

NVIM_CPP_EXPORT [[nodiscard]] bool has_error(const msgpack::object& object)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[nodiscard]] nvim::types::response get_response(
  const nvim::types::connection& connection,
  nvim::types::milliseconds ms = 1000);

NVIM_CPP_EXPORT void get_response_async(
  const nvim::types::connection& connection,
  nvim::types::response_callback callback) NVIM_NOEXCEPT;
}
}

#endif
