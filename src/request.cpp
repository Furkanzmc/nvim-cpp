#include "nvim/request.h"

#include "nvim/exceptions.h"
#include "nvim/private/pack_utils.h"

#include "nvim/generated/nvim_api_generated.h"

#include <msgpack/unpack.hpp>

namespace nvim {
namespace types {

request::_args& operator<<(request::_args& os, int value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os, float value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os, double value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os,
                           const std::string& value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os, std::byte value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os, char value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os, integer_t value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os, uinteger_t value) NVIM_NOEXCEPT
{

    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os,
                           const object_t& value) NVIM_NOEXCEPT
{

    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os,
                           const array_t& value) NVIM_NOEXCEPT
{

    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os,
                           const std::vector<char>& value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);
    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os,
                           const nvim::types::map_t& value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);

    os.count++;

    return os;
}

request::_args& operator<<(request::_args& os,
                           const nvim::types::multimap_t& value) NVIM_NOEXCEPT
{
    msgpack::packer<msgpack::sbuffer> pk{ os.buffer };
    types::impl::pack(pk, value);

    os.count++;

    return os;
}
}
}
