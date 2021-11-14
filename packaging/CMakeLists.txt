include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT DEFINED nvim_cpp_INSTALL_CMAKEDIR)
  set(nvim_cpp_INSTALL_CMAKEDIR
      "${CMAKE_INSTALL_LIBDIR}/cmake/nvim-cpp"
      CACHE STRING "Path to nvim-cpp CMake files")
endif()

install(
  TARGETS nvim-cpp
  EXPORT nvim_cpp_Targets
  RUNTIME COMPONENT nvim_cpp_Runtime
  LIBRARY COMPONENT nvim_cpp_Runtime NAMELINK_COMPONENT nvim_cpp_Development
  ARCHIVE COMPONENT nvim_cpp_Development
  INCLUDES
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(
  DIRECTORY "${nvim_cpp_SOURCE_DIR}/include/" "${nvim_cpp_BINARY_DIR}/include/"
  TYPE INCLUDE
  COMPONENT nvim_cpp_Development)

if(BUILD_SHARED_LIBS)
  set(type shared)
else()
  set(type static)
endif()

install(
  EXPORT nvim_cpp_Targets
  DESTINATION "${nvim_cpp_INSTALL_CMAKEDIR}"
  NAMESPACE nvim-cpp::
  FILE nvim-cpp-${type}-targets.cmake
  COMPONENT nvim_cpp_Development)

write_basic_package_version_file(NvimCppConfigVersion.cmake
                                 COMPATIBILITY SameMajorVersion)
install(
  FILES "${CMAKE_CURRENT_SOURCE_DIR}/NvimCppConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NvimCppConfigVersion.cmake"
  DESTINATION "${nvim_cpp_INSTALL_CMAKEDIR}"
  COMPONENT nvim_cpp_Development)
