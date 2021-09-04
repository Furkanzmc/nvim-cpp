#ifndef NVIM_CPP_EXCEPTIONS_H
#define NVIM_CPP_EXCEPTIONS_H

#include <boost/system/detail/error_code.hpp>

#include <exception>
#include <string.h>
#include <string>

namespace nvim {
namespace exceptions {
class base_error;
class connection_error;
class request_error;
class timeout_error;
class error_category;
class bad_response;
class type_error;
}
}

class nvim::exceptions::error_category : public boost::system::error_category {
public:
    error_category(std::string name, std::string message)
      : m_name{ std::move(name) }
      , m_message{ std::move(message) }
    {
    }

    const char* name() const noexcept override final
    {
        return m_name.c_str();
    }

    std::string message(int) const noexcept override final
    {
        return m_message;
    }

private:
    const std::string m_name;
    const std::string m_message;
};

class nvim::exceptions::base_error : public std::exception {
public:
    base_error(boost::system::error_code ec)
      : std::exception{}
      , m_error_code{ std::move(ec) }
      , m_message{}
    {
    }

    base_error(std::string msg)
      : std::exception{}
      , m_error_code{}
      , m_message{ std::move(msg) }
    {
    }

    const char* what() const noexcept override final
    {
        const auto message = m_error_code ? m_error_code.message() : m_message;
        return strndup(message.c_str(), message.size());
    }

private:
    boost::system::error_code m_error_code;
    std::string m_message;
};

class nvim::exceptions::connection_error : public nvim::exceptions::base_error {
public:
    connection_error(boost::system::error_code ec)
      : base_error{ std::move(ec) }
    {
    }
};

class nvim::exceptions::request_error : public nvim::exceptions::base_error {
public:
    request_error(boost::system::error_code ec)
      : base_error{ std::move(ec) }
    {
    }
};

class nvim::exceptions::timeout_error : public nvim::exceptions::base_error {
public:
    timeout_error(std::string msg = "Request timed out.")
      : base_error{ std::move(msg) }
    {
    }
};

class nvim::exceptions::bad_response : public nvim::exceptions::base_error {
public:
    bad_response(std::string msg)
      : base_error{ std::move(msg) }
      , m_error_code(0)
    {
    }

    bad_response(std::string msg, std::int64_t error_code)
      : base_error{ std::move(msg) }
      , m_error_code(error_code)
    {
    }

    std::int64_t error_code() const noexcept
    {
        return m_error_code;
    }

private:
    std::int64_t m_error_code;
};

class nvim::exceptions::type_error : public nvim::exceptions::base_error {
public:
    type_error(std::string msg)
      : base_error{ std::move(msg) }
    {
    }
};

#endif
