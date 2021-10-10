#include "nvim/api.h"
#include "nvim/connection.h"
#include "nvim/exceptions.h"
#include "nvim/generated/nvim_api_generated.h"
#include "nvim/request.h"
#include "nvim/response.h"
#include "nvim/private/event_store.h"

#include "test_utils.h"

#include <msgpack/unpack.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/dispatch.hpp>

#include <catch2/catch_test_macros.hpp>

#include <iostream>

using namespace boost;

namespace {
bool handle_events() noexcept
{
    bool consumed{ false };
    while (nvim::api::detail::has_event()) {
        auto event = nvim::api::detail::consume_event();
        std::visit(
          [&consumed](auto&& arg) {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, nvim::ui_events::grid_line>) {
                  consumed = true;
              }
          },
          event);
    }

    return consumed;
}
}

SCENARIO("A function with return value is called.", "[nvim::generated_api]")
{
    nvim_process nr;

    nvim::types::connection connection{ std::move(
      nvim::connect_tcp("127.0.0.1", 6667)) };

    REQUIRE(connection.socket->is_open());
    WHEN("nvim_get_current_bufs called.")
    {
        auto bufnr = nvim::api::nvim_get_current_buf(connection);
        REQUIRE(bufnr > 0);
    }

    WHEN("nvim_buf_get_commands is called.")
    {
        auto bufnr = nvim::api::nvim_get_current_buf(connection);
        REQUIRE(bufnr > 0);
        nvim::api::nvim_command(
          connection, "command! -buffer CustomCommand <buffer> :echo 'Hello'");

        auto command_map =
          nvim::api::nvim_buf_get_commands(connection, bufnr, {});
        REQUIRE(command_map.size() > 0);
    }

    WHEN("nvim_buf_get_lines is called.")
    {
        const auto bufnr = nvim::api::nvim_get_current_buf(connection);
        REQUIRE(bufnr > 0);

        nvim::api::nvim_buf_set_lines(
          connection, bufnr, 0, 0, true, { "Hello", "Wordl!" });

        const auto lines =
          nvim::api::nvim_buf_get_lines(connection, bufnr, 0, 2, true);
        REQUIRE(!lines.empty());
    }
}

SCENARIO("An API error is raised.", "[nvim::generated_api]")
{
    nvim_process nr;
    nvim::types::connection connection{ std::move(
      nvim::connect_tcp("127.0.0.1", 6667)) };

    REQUIRE(connection.socket->is_open());

    nvim::api::nvim_command(connection,
                            "command DuplicateCommand :echo 'Hello'");

    int error_ocurred{ -1 };
    nvim::api::nvim_command(connection,
                            "command DuplicateCommand :echo 'Hello'",
                            [&error_ocurred](const auto& response) {
                                error_ocurred = static_cast<int>(
                                  response.error.message.size());
                            });

    CHECK_THROWS_AS(nvim::api::nvim_command(
                      connection, "command DuplicateCommand :echo 'Hello'"),
                    nvim::exceptions::bad_response);
}

SCENARIO("An async API function with return value is called.",
         "[nvim::generated_api]")
{
    nvim_process nr;
    nvim::types::connection connection{ std::move(
      nvim::connect_tcp("127.0.0.1", 6667)) };

    REQUIRE(connection.socket->is_open());

    bool response_read{ false };
    nvim::api::nvim_get_current_buf(
      connection, [&response_read](const auto& response) {
          response_read = true;
          const nvim::types::buffer_t bufnr = response.value;
          REQUIRE(bufnr > 0);
          std::clog << "bufnr = " << bufnr << '\n';
      });

    nvim::api::detail::start_loop(
      connection,
      [&connection, &response_read]() {
          if (response_read) {
              nvim::api::detail::stop_loop(connection);
          }
      },
      [](const auto& error_code) {
          std::cerr << "Error: " << error_code.failed() << '\n';
      });

    REQUIRE(response_read);
}

SCENARIO("Attach to a running nvim.", "[nvim::generated_api]")
{
    nvim_process nr;
    nvim::types::connection connection{ std::move(
      nvim::connect_tcp("127.0.0.1", 6667)) };

    REQUIRE(connection.socket->is_open());

    nvim::types::multimap_t options{
        { "rgb", true },
        { "ext_cmdline", true },
        { "ext_hlstate", true },
        { "ext_linegrid", true },
        { "ext_messages", true },
        { "ext_multigrid", true },
        { "ext_popupmenu", true },
        { "ext_tabline", true },
        { "ext_termcolors", true },
    };
    nvim::api::nvim_ui_attach(connection, 100, 100, options);

    nvim::api::detail::start_loop(
      connection,
      [&connection]() {
          if (handle_events()) {
              nvim::api::detail::stop_loop(connection);
          }
      },
      [](const auto& error_code) {
          std::cerr << "Error: " << error_code.failed() << '\n';
      });
}
