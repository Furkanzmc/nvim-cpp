cmake_minimum_required(VERSION 3.14)

project(nvim-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(NVIM_CPP_ENABLE_TESTS "Build and run tests." ON)

set(PROJECT_SOURCES
  src/connection.cpp
  src/api.cpp
  src/response.cpp
  src/request.cpp
  src/private/pack_utils.cpp
  src/private/event_store.cpp
  )

find_package(PythonInterp)
if (PYTHONINTERP_FOUND)
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/generate_api.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_QUIET
    COMMAND_ECHO NONE
    )
else()
  message(FATAL "Python executable is not found.")
endif(PYTHONINTERP_FOUND)

list(APPEND PROJECT_SOURCES src/generated/nvim_api_generated.cpp)

add_library(nvim-cpp STATIC ${PROJECT_SOURCES})
add_library(nvim::cpp ALIAS nvim-cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  target_compile_options(nvim-cpp PRIVATE -Wall -Wextra -pedantic -Werror -std=c++20)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(STATUS "No options set for GNU.")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(nvim-cpp PRIVATE /W4 /WX)
endif()

target_include_directories(nvim-cpp PRIVATE
  include/
  )

add_subdirectory(third-party/)

target_include_directories(nvim-cpp
  PRIVATE ${MSGPACK_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

if (NVIM_CPP_ENABLE_TESTS)
  add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
    COMMENT "Build and run all the tests and examples.")

  add_subdirectory(test/)
endif()

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        8.0.1
  )

FetchContent_MakeAvailable(fmt)

target_link_libraries(nvim-cpp
  PRIVATE ${MSGPACK_LIBRARIES} fmt::fmt-header-only)
