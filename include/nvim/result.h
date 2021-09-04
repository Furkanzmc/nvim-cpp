#ifndef NVIM_CPP_RESULT_H
#define NVIM_CPP_RESULT_H

#include <functional>
#include <optional>
#include <string>
#include <type_traits>

namespace nvim {
struct Error {
    std::string message;
};

template<class T,
         class E = Error,
         typename std::enable_if<
           (std::is_move_constructible<T>{} && std::is_move_assignable<T>{}) ||
             (std::is_trivially_constructible<T>::value &&
              std::is_trivially_copy_assignable<T>::value &&
              std::is_trivially_copyable<T>{}),
           bool>::type = false>
class result {

    using error_callback = std::function<void(const E&)>;
    using success_callback = std::function<void(const T&)>;

private:
    template<typename... T2>
    struct match {
        explicit match(T2&&... rest)
          : m_val{ std::forward<T2>(rest)... }
        {
        }

        operator T&&()
        {
            return std::forward<T>(m_val);
        }

    private:
        T m_val;
    };

public:
    template<typename... T3>
    [[nodiscard]] result(T3&&... rest)
      : m_match{ std::forward<T3>(rest)... }
    {
    }

    template<typename... T3>
    [[nodiscard]] result(E _err, T3&&... defaultVal)
      : m_err{ std::move(_err) }
      , m_match{ std::forward<T3>(defaultVal)... }
    {
    }

    [[nodiscard]] match<T> expect(error_callback err,
                                  success_callback ok = nullptr)
    {
        if (m_err != std::nullopt) {
#ifdef __cpp_exceptions
            if (!err) {
                throw std::invalid_argument{ "result errors must be handled." };
            }
#else
            assert(err && "result errors must be handled.");
#endif

            err(this->m_err.value());
        }
        else if (ok) {
            ok(m_match);
        }

        return std::move(m_match);
    }

private:
    std::optional<E> m_err;
    match<T> m_match;
};
}

#endif
