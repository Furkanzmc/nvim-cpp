#ifndef NVIM_CPP_REQUEST_H
#define NVIM_CPP_REQUEST_H

#include "nvim/types.h"
#include "nvim/defs.h"
#include "nvim/export.h"

namespace nvim::types {

struct NVIM_CPP_EXPORT request {
    struct _args {
        std::uint32_t count;
        msgpack::sbuffer& buffer;
    };

    msgpack::sbuffer buffer;
    const std::uint32_t arg_count;
    const std::uint64_t msg_id;

    write_callback on_bytes_written{};

    _args args{ 0, buffer };
};

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(request::_args& os,
                                                            int value)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(request::_args& os,
                                                            float value)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(request::_args& os,
                                                            double value)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(
  request::_args& os,
  const std::string& value) NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(request::_args& os,
                                                            std::byte value)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(request::_args& os,
                                                            char value)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(request::_args& os,
                                                            integer_t value)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(request::_args& os,
                                                            uinteger_t value)
  NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(
  request::_args& os,
  const object_t& value) NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(
  request::_args& os,
  const array_t& value) NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(
  request::_args& os,
  const std::vector<char>& value) NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(
  request::_args& os,
  const nvim::types::map_t& value) NVIM_NOEXCEPT;

NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(
  request::_args& os,
  const nvim::types::map_t& value) NVIM_NOEXCEPT;

template<typename T>
NVIM_CPP_EXPORT [[maybe_unused]] request::_args& operator<<(
  request::_args& os,
  const std::vector<T>& value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    const auto value_size = static_cast<std::uint32_t>(value.size());
    pk.pack_array(value_size);

    for (const auto& entry : value) {
        os << entry;
    }

    os.count -= value_size;
    os.count++;
    return os;
}

}

#endif
