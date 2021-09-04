#include "nvim/private/pack_utils.h"

namespace nvim {
namespace types {
namespace impl {

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::object_t& value) NVIM_NOEXCEPT
{
    if (value.is_vector_char()) {
        impl::pack(pk, value.as_vector_char());
    }
    else if (value.is_int64_t()) {
        impl::pack(pk, value.as_int64_t());
    }
    else if (value.is_uint64_t()) {
        impl::pack(pk, value.as_uint64_t());
    }
    else if (value.is_double()) {
        impl::pack(pk, value.as_double());
    }
    else if (value.is_string()) {
        impl::pack(pk, value.as_string());
    }
    else if (value.is_vector()) {
        impl::pack(pk, value.as_vector());
    }
    else if (value.is_bool()) {
        impl::pack(pk, value.as_bool());
    }
    else if (value.is_map()) {
        impl::pack(pk, value.as_map());
    }
    else if (value.is_multimap()) {
        impl::pack(pk, value.as_multimap());
    }
    else {
        assert("Wrong type." == 0);
    }
}

void pack(msgpack::packer<msgpack::sbuffer>& pk, int value) NVIM_NOEXCEPT
{
    pk.pack_int(value);
}

void pack(msgpack::packer<msgpack::sbuffer>& pk, bool value) NVIM_NOEXCEPT
{
    if (value) {
        pk.pack_true();
    }
    else {
        pk.pack_false();
    }
}

void pack(msgpack::packer<msgpack::sbuffer>& pk, float value) NVIM_NOEXCEPT
{
    pk.pack_float(value);
}

void pack(msgpack::packer<msgpack::sbuffer>& pk, double value) NVIM_NOEXCEPT
{
    pk.pack_double(value);
}

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const std::string& value) NVIM_NOEXCEPT
{
    pk.pack_str(value.size());
    pk.pack_str_body(value.c_str(), value.size());
}

void pack(msgpack::packer<msgpack::sbuffer>& pk, std::byte value) NVIM_NOEXCEPT
{
    pk.pack_unsigned_char(static_cast<char>(value));
}

void pack(msgpack::packer<msgpack::sbuffer>& pk, char value) NVIM_NOEXCEPT
{
    pk.pack_char(value);
}

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          nvim::types::integer_t value) NVIM_NOEXCEPT
{
    pk.pack_int64(value);
}

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          nvim::types::uinteger_t value) NVIM_NOEXCEPT
{
    pk.pack_uint64(value);
}

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::array_t& value) NVIM_NOEXCEPT
{
    pk.pack_array(value.size());
    for (const nvim::types::object_t& var : value) {
        pack(pk, var);
    }
}

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const std::vector<char>& value) NVIM_NOEXCEPT
{
    pk.pack_bin(value.size());
    pk.pack_bin_body(value.data(), value.size());
}

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::map_t& value) NVIM_NOEXCEPT
{
    pk.pack_map(value.size());
    for (const auto& pair : value) {
        pack(pk, pair.first);
        pack(pk, pair.second);
    }
}

void pack(msgpack::packer<msgpack::sbuffer>& pk,
          const nvim::types::multimap_t& value) NVIM_NOEXCEPT
{
    pk.pack_map(value.size());
    for (const auto& pair : value) {
        pack(pk, pair.first);
        pack(pk, pair.second);
    }
}

}
}
}
