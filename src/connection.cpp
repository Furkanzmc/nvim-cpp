#include "nvim/connection.h"

#include "nvim/exceptions.h"

#include <boost/asio/io_context.hpp>

using namespace boost;

namespace nvim {

types::connection connect_tcp(const std::string& path, int port)
{
    auto io_context{ std::make_unique<asio::io_context>() };
    std::unique_ptr<asio::ip::tcp::socket> socket{ new asio::ip::tcp::socket{
      *io_context } };
    asio::ip::tcp::endpoint endpoint{ asio::ip::address::from_string(path),
                                      static_cast<asio::ip::port_type>(port) };
    system::error_code ec;
    socket->connect(endpoint, ec);
    if (ec) {
        socket->close();
        throw nvim::exceptions::connection_error{ ec };
    }

    return types::connection{ std::move(socket), std::move(io_context) };
}
}
