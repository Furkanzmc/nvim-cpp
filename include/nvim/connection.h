#ifndef NVIM_CPP_CONNECTION_H
#define NVIM_CPP_CONNECTION_H

#include "nvim/defs.h"
#include "nvim/export.h"

#include <boost/asio/ip/tcp.hpp>

namespace boost::asio {
class io_context;
}

namespace nvim {
namespace types {
struct NVIM_CPP_EXPORT connection {
    std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    std::unique_ptr<boost::asio::io_context> io_context;
};
}
NVIM_CPP_EXPORT types::connection connect_tcp(const std::string& path,
                                              int port);
}

#endif
