#ifndef NVIM_CPP_RESPONSE_H
#define NVIM_CPP_RESPONSE_H

#include "nvim/types.h"
#include "nvim/defs.h"

#include <string>

namespace nvim::types {

struct response {
    struct error {
        const std::string message{};
        const std::int64_t code{ -1 };
    };

    struct _value {
        const types::object_t value;

        [[nodiscard]] operator std::nullptr_t() const NVIM_NOEXCEPT;
        [[nodiscard]] operator bool_t() const NVIM_NOEXCEPT;
        [[nodiscard]] operator int() const NVIM_NOEXCEPT;
        [[nodiscard]] operator integer_t() const NVIM_NOEXCEPT;
        [[nodiscard]] operator uinteger_t() const NVIM_NOEXCEPT;
        [[nodiscard]] operator double_t() const NVIM_NOEXCEPT;
        [[nodiscard]] operator const string_t&() const NVIM_NOEXCEPT;
        [[nodiscard]] operator std::vector<char>() const NVIM_NOEXCEPT;
        [[nodiscard]] operator std::vector<std::string>() const NVIM_NOEXCEPT;
        [[nodiscard]] operator std::vector<integer_t>() const NVIM_NOEXCEPT;
        [[nodiscard]] operator std::vector<uinteger_t>() const NVIM_NOEXCEPT;
        [[nodiscard]] operator std::vector<map_t>() const NVIM_NOEXCEPT;
        [[nodiscard]] operator std::vector<multimap_t>() const NVIM_NOEXCEPT;
        [[nodiscard]] operator const map_t&() const NVIM_NOEXCEPT;
        [[nodiscard]] operator const multimap_t&() const NVIM_NOEXCEPT;
        [[nodiscard]] operator const array_t&() const NVIM_NOEXCEPT;
        [[nodiscard]] operator const object_t&() const NVIM_NOEXCEPT;

        [[nodiscard]] const object_t* operator->() const NVIM_NOEXCEPT;
        [[nodiscard]] const object_t& operator*() const NVIM_NOEXCEPT;

        template<typename T>
        bool operator==(const T& other) const NVIM_NOEXCEPT
        {
            return static_cast<T>(*this) == other;
        }
    };

    const types::integer_t msg_id;
    const types::message_type msg_type;
    const _value value;
    const error error;
    const types::string_t notification;
};
}
#endif
