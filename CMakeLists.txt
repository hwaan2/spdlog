#
# Copyright(c) 2015 Ruslan Baratov.
# Distributed under the MIT License (http://opensource.org/licenses/MIT)
#

cmake_minimum_required(VERSION 3.1)
include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.20.64.tar.gz"
    SHA1 "7b830dfda7a094b2af15f44b24ebd2489404d880"
)
project(spdlog VERSION 1.2.1 LANGUAGES CXX)
include(CTest)
include(CMakeDependentOption)
include(GNUInstallDirs)

hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)
#---------------------------------------------------------------------------------------
# set default build to release
#---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

#---------------------------------------------------------------------------------------
# compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options("-Wall")
    add_compile_options("-Wextra")
    add_compile_options("-Wconversion")
    add_compile_options("-pedantic")
    add_compile_options("-Wfatal-errors")

endif()

#---------------------------------------------------------------------------------------
# address sanitizers check
#---------------------------------------------------------------------------------------
include(cmake/sanitizers.cmake)

#---------------------------------------------------------------------------------------
# spdlog target
#---------------------------------------------------------------------------------------
add_library(spdlog INTERFACE)
add_library(spdlog::spdlog ALIAS spdlog)

# Check if spdlog is being used directly or via add_subdirectory
set(SPDLOG_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(SPDLOG_MASTER_PROJECT ON)
endif()

option(SPDLOG_BUILD_EXAMPLES "Build examples" ${SPDLOG_MASTER_PROJECT})
option(SPDLOG_BUILD_BENCH "Build benchmarks" ${SPDLOG_MASTER_PROJECT})

cmake_dependent_option(SPDLOG_BUILD_TESTING
    "Build spdlog tests" ${SPDLOG_MASTER_PROJECT}
    "BUILD_TESTING" OFF
)

target_include_directories(
    spdlog
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(spdlog INTERFACE fmt::fmt)
target_compile_definitions(spdlog INTERFACE SPDLOG_FMT_EXTERNAL=1)

if(SPDLOG_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(SPDLOG_BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(SPDLOG_BUILD_BENCH)
    add_subdirectory(bench)
endif()

### Install ###
# * https://github.com/forexample/package-example
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")
set(pkgconfig_install_dir "lib/pkgconfig")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(pkg_config "${generated_dir}/${PROJECT_NAME}.pc")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

configure_file("cmake/spdlog.pc.in" "${pkg_config}" @ONLY)

# Note: use 'targets_export_name'
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

install(
    TARGETS spdlog
    EXPORT "${targets_export_name}"
    INCLUDES DESTINATION "${include_install_dir}"
)

install(DIRECTORY "include/spdlog" DESTINATION "${include_install_dir}")

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

install(
    FILES "${pkg_config}"
    DESTINATION "${pkgconfig_install_dir}"
)

install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)

file(GLOB_RECURSE spdlog_include_SRCS "${HEADER_BASE}/*.h")
add_custom_target(spdlog_headers_for_ide SOURCES ${spdlog_include_SRCS})
