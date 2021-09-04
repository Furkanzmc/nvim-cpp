#ifndef NVIM_CPP_TYPES_H
#define NVIM_CPP_TYPES_H

#include "nvim/defs.h"

#include <msgpack/object.hpp>
#include <msgpack/type.hpp>
#include <msgpack/v3/cpp_config_decl.hpp>
#include <msgpack/sbuffer.hpp>

#include <memory>
#include <functional>
#include <string>
#include <map>

namespace boost {
namespace system {
class error_code;
}
}

namespace nvim {

namespace types {
struct response;

using bool_t = bool;
using integer_t = std::int64_t;
using uinteger_t = std::uint64_t;
using float_t = float;
using window_t = uinteger_t;
using buffer_t = uinteger_t;
using tab_page_t = uinteger_t;
using double_t = double;
using string_t = std::string;
using object_t = msgpack::type::variant;
using map_t = std::map<nvim::types::object_t, nvim::types::object_t>;
using multimap_t = std::multimap<nvim::types::object_t, nvim::types::object_t>;
using milliseconds = unsigned long;
using in_buffer = char;
using array_t = std::vector<nvim::types::object_t>;

using read_callback =
  std::function<void(const boost::system::error_code& /*error*/,
                     std::size_t /*bytes_read*/)>;

using wait_callback =
  std::function<void(const boost::system::error_code& /*error*/)>;

using loop_callback = std::function<void()>;

using error_callback =
  std::function<void(const boost::system::error_code& /*error*/)>;

using write_callback =
  std::function<void(const boost::system::error_code& /*error*/,
                     std::size_t /*bytes_read*/)>;

using response_callback = std::function<void(const response&)>;

enum class message_type : std::uint8_t {
    request = 0,
    response = 1,
    notification = 2
};
}
}

#endif
