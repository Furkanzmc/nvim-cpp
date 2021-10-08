#include <boost/process/child.hpp>
#include <catch2/catch_test_macros.hpp>

#include <chrono>

struct nvim_process {
    boost::process::child nvim{ NVIM_PATH,       "-u",         "NORC",
                                "--noplugins",   "--headless", "--listen",
                                "127.0.0.1:6667" };

    nvim_process() noexcept
    {
        REQUIRE(nvim.running());
        nvim.wait_for(std::chrono::milliseconds{ 1000 });
    }

    ~nvim_process() noexcept
    {
        REQUIRE(nvim.running());
    }

    [[nodiscard]] boost::process::child* operator->() noexcept
    {
        return &nvim;
    }
};
