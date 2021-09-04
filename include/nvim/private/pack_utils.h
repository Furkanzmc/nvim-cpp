#ifndef NVIM_CPP_PACK_UTILS_H
#define NVIM_CPP_PACK_UTILS_H

#include "nvim/types.h"
#include "nvim/defs.h"

namespace nvim {
namespace types {
namespace impl {

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::object_t& value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk, int value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk, bool value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk, float value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk, double value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const std::string& value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk, std::byte value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk, char value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          nvim::types::integer_t value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          nvim::types::uinteger_t value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::array_t& value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const std::vector<char>& value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::map_t& value) NVIM_NOEXCEPT;

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::multimap_t& value) NVIM_NOEXCEPT;

}
}
}

#endif
