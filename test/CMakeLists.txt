set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(NVIM_PATH nvim REQUIRED)
add_compile_definitions(NVIM_PATH="${NVIM_PATH}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.0-preview3)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

set(TEST_LIBRARIES Catch2::Catch2WithMain nvim-cpp::nvim-cpp ${MSGPACK_LIBRARIES})
set(TEST_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} ../include/)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(Boost_DEBUG ON)
else()
  set(Boost_DEBUG OFF)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(TEST_COMPILER_FLAGS "-Wall -Wextra -pedantic -Werror -std=c++20")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  list(APPEND TEST_LIBRARIES Threads::Threads)

  set(TEST_COMPILER_FLAGS "-Wall -Wextra -pedantic -Werror -std=c++2a")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(TEST_COMPILER_FLAGS "/W4 /WX")
  # NOTE: We need to have this here so that boost picks the correct io_context implementation. See:
  # https://stackoverflow.com/a/41068811
  set(TEST_COMPILE_DEFINITIONS "-D_WIN32_WINNT=0x0A00 -D_WIN32")

  set(Boost_USE_STATIC_LIBS ON)
  set(BOOST_ROOT ${_BOOST_ROOT})
  find_package(Boost REQUIRED COMPONENTS filesystem)
endif()

link_directories(${Boost_LIBRARY_DIRS})

add_executable(connection_tests connection.cpp)
add_executable(api_tests api.cpp)
add_executable(generated_api_tests generated_api.cpp)

target_compile_definitions(connection_tests PUBLIC ${TEST_COMPILE_DEFINITIONS})
target_compile_definitions(api_tests PUBLIC ${TEST_COMPILE_DEFINITIONS})
target_compile_definitions(generated_api_tests PUBLIC ${TEST_COMPILE_DEFINITIONS})

target_compile_options(connection_tests PRIVATE ${TEST_COMPILER_FLAGS_STR})
target_compile_options(api_tests PRIVATE ${TEST_COMPILER_FLAGS_STR})
target_compile_options(generated_api_tests PRIVATE ${TEST_COMPILER_FLAGS_STR})

target_link_libraries(connection_tests PRIVATE ${TEST_LIBRARIES})
target_link_libraries(api_tests PRIVATE ${TEST_LIBRARIES})
target_link_libraries(generated_api_tests PRIVATE ${TEST_LIBRARIES})

target_include_directories(api_tests PRIVATE ${TEST_INCLUDE_DIRS})
target_include_directories(connection_tests PRIVATE ${TEST_INCLUDE_DIRS})
target_include_directories(generated_api_tests PRIVATE ${TEST_INCLUDE_DIRS})

catch_discover_tests(connection_tests REPORTER)
catch_discover_tests(api_tests REPORTER)
catch_discover_tests(generated_api_tests REPORTER)
