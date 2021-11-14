cmake_minimum_required(VERSION 3.19)

project(
  nvim-cpp
  VERSION 0.1.0
  LANGUAGES CXX)

if(NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND NOT DEFINED
                                               CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif()

if(DEFINED nvim_cpp_SHARED_LIBS)
  set(BUILD_SHARED_LIBS "${nvim_cpp_SHARED_LIBS}")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_SOURCES
    src/connection.cpp
    src/api.cpp
    src/response.cpp
    src/request.cpp
    src/private/pack_utils.cpp
    src/private/event_store.cpp
    src/generated/nvim_api_generated.cpp)

option(NVIM_CPP_ENABLE_TESTS "Build and run tests." ON)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} -m pip install -r
            ${PROJECT_SOURCE_DIR}/scripts/requirements.txt
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_QUIET COMMAND_ECHO NONE)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/generate_api.py
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_QUIET COMMAND_ECHO NONE)
else()
  message(FATAL "Python executable is not found.")
endif(Python3_FOUND)

add_library(nvim-cpp STATIC ${PROJECT_SOURCES})
add_library(nvim-cpp::nvim-cpp ALIAS nvim-cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  target_compile_options(nvim-cpp PRIVATE -Wall -Wextra -pedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(nvim-cpp PRIVATE -Wall -Wextra -pedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(nvim-cpp PRIVATE /W4 /WX)
  target_compile_definitions(nvim-cpp PUBLIC -D_WIN32 -D_WIN32_WINNT=0x0A00)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(Boost_DEBUG ON)
else()
  set(Boost_DEBUG OFF)
endif()

if((NVIM_CPP_ENABLE_TESTS) AND (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
  set(_BOOST_BUILD_COMPONENTS
      filesystem
      CACHE INTERNAL "Boost libraries to build.")
endif()

add_subdirectory(third-party/)

file(RELATIVE_PATH MSGPACK_REL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}
     ${MSGPACK_INCLUDE_DIRS})
file(RELATIVE_PATH BOOST_REL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}
     ${Boost_INCLUDE_DIR})

target_include_directories(
  nvim-cpp
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${MSGPACK_REL_INCLUDE}>"
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${BOOST_REL_INCLUDE}>")

include(GenerateExportHeader)
generate_export_header(nvim-cpp EXPORT_FILE_NAME include/nvim/export.h)
target_compile_definitions(
  nvim-cpp
  PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:NVIM_CPP_STATIC_DEFINE>")
target_include_directories(
  nvim-cpp PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
               is_top_level)
option(nvim_cpp_INCLUDE_PACKAGING "Include packaging rules for nvim-cpp"
       "${is_top_level}")
if(nvim_cpp_INCLUDE_PACKAGING)
  add_subdirectory(packaging/)
endif()

if(NVIM_CPP_ENABLE_TESTS)
  add_custom_target(
    check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
    COMMENT "Build and run all the tests and examples.")

  add_subdirectory(test/)
endif()
