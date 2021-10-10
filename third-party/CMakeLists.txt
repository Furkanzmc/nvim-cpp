# This is a minimal CMake project to fetch and build third party
# dependencies

cmake_minimum_required(VERSION 3.14)
if (POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(nvim-cpp-deps)

set(NVIM_CPP_THIRD_PARTY_ARTIFACTS ${CMAKE_CURRENT_SOURCE_DIR}/artifacts)
set(MSGPACK_USE_STATIC_BOOST OFF)

# boost {{{

set(BOOST_VERSION 1.77.0)
string(REPLACE "." "_" BOOST_UNDERSCORE_VERSION "${BOOST_VERSION}")
set(BOOST_TARBALL boost_${BOOST_UNDERSCORE_VERSION}.tar.gz)
set(BOOST_FOLDER_NAME boost_${BOOST_UNDERSCORE_VERSION})

set(BOOST_URL https://boostorg.jfrog.io/artifactory/main/release/${BOOST_VERSION}/source/boost_${BOOST_UNDERSCORE_VERSION}.tar.gz)
set(BOOST_SHA256 5347464af5b14ac54bb945dc68f1dd7c56f0dad7262816b956138fc53bcc0131)

if(EXISTS ${NVIM_CPP_THIRD_PARTY_ARTIFACTS}/${BOOST_FOLDER_NAME})
	message(STATUS "Boost already exists. Skipping download...")
else()
	message(STATUS "Downloading boost...")
	file(DOWNLOAD ${BOOST_URL} ${NVIM_CPP_THIRD_PARTY_ARTIFACTS}/${BOOST_TARBALL}
		INACTIVITY_TIMEOUT 30
		EXPECTED_HASH SHA256=${BOOST_SHA256})

	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${BOOST_TARBALL}
		WORKING_DIRECTORY ${NVIM_CPP_THIRD_PARTY_ARTIFACTS}
		RESULT_VARIABLE rv)

	if(NOT rv EQUAL 0)
		message(FATAL_ERROR "Failed to extract ${BOOST_TARBALL}")
	endif()
endif()

set(BOOST_ROOT
	${NVIM_CPP_THIRD_PARTY_ARTIFACTS}/${BOOST_FOLDER_NAME}
	CACHE
	INTERNAL
	"Boost path"
	TRUE)

# }}}

# Msgpack {{{

set(MSGPACK_VERSION 4.0.2)
set(MSGPACK_URL https://github.com/msgpack/msgpack-c/archive/cpp-${MSGPACK_VERSION}.tar.gz)
set(MSGPACK_SHA256 359393de94da614896974b99ef25047c0536bbb2c265be9b7c898c1ace38ff3f)

set(MSGPACK_TARBALL msgpack-${MSGPACK_VERSION}.tar.gz)

if(EXISTS ${NVIM_CPP_THIRD_PARTY_ARTIFACTS}/${MSGPACK_TARBALL})
	message(STATUS "msgpack already exists. Skipping download...")
else()
	message(STATUS "Downloading Msgpack...")
	file(DOWNLOAD ${MSGPACK_URL} ${NVIM_CPP_THIRD_PARTY_ARTIFACTS}/${MSGPACK_TARBALL}
		INACTIVITY_TIMEOUT 30
		EXPECTED_HASH SHA256=${MSGPACK_SHA256})

	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${MSGPACK_TARBALL}
		WORKING_DIRECTORY ${NVIM_CPP_THIRD_PARTY_ARTIFACTS}
		RESULT_VARIABLE rv)
	if(NOT rv EQUAL 0)
		message(FATAL_ERROR "Failed to extract ${MSGPACK_TARBALL}")
	endif()
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(MSGPACK_SOURCE_DIR ${NVIM_CPP_THIRD_PARTY_ARTIFACTS}/msgpack-c-cpp-${MSGPACK_VERSION}/)
add_subdirectory(${MSGPACK_SOURCE_DIR} EXCLUDE_FROM_ALL)

set(MSGPACK_INCLUDE_DIRS ${MSGPACK_SOURCE_DIR}/include PARENT_SCOPE)

# }}}
