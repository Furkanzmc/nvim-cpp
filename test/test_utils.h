#include <boost/process/child.hpp>
#include <catch2/catch_test_macros.hpp>

#include <chrono>

struct nvim_process {
    boost::process::child nvim{ NVIM_PATH,
                                "--headless",
                                "--listen",
                                "127.0.0.1:6667" };

    nvim_process()
    {
        REQUIRE(nvim.running());
        nvim.wait_for(std::chrono::milliseconds{ 1000 });
    }

    ~nvim_process()
    {
        REQUIRE(nvim.running());
    }

    boost::process::child* operator->()
    {
        return &nvim;
    }
};
