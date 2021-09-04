#include "nvim/api.h"
#include "nvim/connection.h"
#include "nvim/exceptions.h"

#include "test_utils.h"

#include <msgpack/unpack.hpp>

#include <catch2/catch_test_macros.hpp>

#include <iostream>

SCENARIO("A connection can be created with an IP and port.",
         "[nvim::types::connection]")
{
    nvim_process np;

    GIVEN("An IP and a port")
    {
        nvim::types::connection connection{};
        CHECK_NOTHROW(connection =
                        std::move(nvim::connect_tcp("127.0.0.1", 6667)));

        WHEN("Connection is established.")
        {
            REQUIRE(connection.socket->is_open());
        }
    }
}
