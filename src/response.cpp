#include "nvim/response.h"

#include "nvim/generated/nvim_api_generated.h"
#include "nvim/exceptions.h"

namespace nvim::types {

response::_value::operator std::nullptr_t() const NVIM_NOEXCEPT
{
    assert(value.is_nil());
    return nullptr;
}

response::_value::operator bool_t() const NVIM_NOEXCEPT
{
    assert(value.is_bool());
    return value.as_bool();
}

response::_value::operator int() const NVIM_NOEXCEPT
{
    assert(value.is_int64_t() || value.is_uint64_t() || value.is_ext());
    if (value.is_ext()) {
        return static_cast<int>(nvim::types::detail::unpack_ext(value));
    }

    if (value.is_int64_t()) {
        return static_cast<int>(value.as_int64_t());
    }

    return static_cast<int>(value.as_uint64_t());
}

response::_value::operator integer_t() const NVIM_NOEXCEPT
{
    assert(value.is_int64_t() || value.is_ext());
    if (value.is_ext()) {
        return static_cast<integer_t>(nvim::types::detail::unpack_ext(value));
    }

    return value.as_int64_t();
}

response::_value::operator uinteger_t() const NVIM_NOEXCEPT
{
    assert(value.is_uint64_t() || value.is_ext());
    if (value.is_ext()) {
        return nvim::types::detail::unpack_ext(value);
    }

    return value.as_uint64_t();
}

response::_value::operator double_t() const NVIM_NOEXCEPT
{
    assert(value.is_double());
    return value.as_double();
}

response::_value::operator const string_t&() const NVIM_NOEXCEPT
{
    assert(value.is_string());
    return value.as_string();
}

response::_value::operator std::vector<char>() const NVIM_NOEXCEPT
{
    assert(value.is_vector_char());
    return value.as_vector_char();
}

response::_value::operator std::vector<std::string>() const NVIM_NOEXCEPT
{
    assert(value.is_vector());

    std::vector<std::string> strings;
    const auto var_vector = value.as_vector();
    for (const auto& var : var_vector) {
        assert(var.is_string());
        strings.push_back(var.as_string());
    }

    return strings;
}

response::_value::operator std::vector<integer_t>() const NVIM_NOEXCEPT
{
    assert(value.is_vector());

    std::vector<integer_t> ints;
    const auto var_vector = value.as_vector();
    for (const auto& var : var_vector) {
        assert(var.is_int64_t());
        ints.push_back(var.as_int64_t());
    }

    return ints;
}

response::_value::operator std::vector<uinteger_t>() const NVIM_NOEXCEPT
{
    assert(value.is_vector());

    std::vector<uinteger_t> ints;
    const auto var_vector = value.as_vector();
    for (const auto& var : var_vector) {
        assert(var.is_uint64_t());
        ints.push_back(var.as_uint64_t());
    }

    return ints;
}

response::_value::operator std::vector<map_t>() const NVIM_NOEXCEPT
{
    assert(value.is_vector());

    std::vector<map_t> maps;
    const auto var_vector = value.as_vector();
    for (const auto& var : var_vector) {
        (void)var;
        assert(false);
    }

    return maps;
}

response::_value::operator std::vector<multimap_t>() const NVIM_NOEXCEPT
{
    assert(value.is_vector());

    std::vector<multimap_t> maps;
    const auto var_vector = value.as_vector();
    for (const auto& var : var_vector) {
        (void)var;
        assert(false);
    }

    return maps;
}

response::_value::operator const map_t&() const NVIM_NOEXCEPT
{
    assert(value.is_map());

    return value.as_map();
}

response::_value::operator const multimap_t&() const NVIM_NOEXCEPT
{
    assert(value.is_multimap());

    return value.as_multimap();
}

response::_value::operator const array_t&() const NVIM_NOEXCEPT
{
    assert(value.is_vector());

    return value.as_vector();
}

response::_value::operator const object_t&() const NVIM_NOEXCEPT
{
    return value;
}

const object_t* response::_value::operator->() const NVIM_NOEXCEPT
{
    return &value;
}

const object_t& response::_value::operator*() const NVIM_NOEXCEPT
{
    return value;
}

}
