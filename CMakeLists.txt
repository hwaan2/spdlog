# *************************************************************************/
# * Copyright (c) 2015 Ruslan Baratov.                                    */
# *                                                                       */
# * Permission is hereby granted, free of charge, to any person obtaining */
# * a copy of this software and associated documentation files (the       */
# * "Software"), to deal in the Software without restriction, including   */
# * without limitation the rights to use, copy, modify, merge, publish,   */
# * distribute, sublicense, and/or sell copies of the Software, and to    */
# * permit persons to whom the Software is furnished to do so, subject to */
# * the following conditions:                                             */
# *                                                                       */
# * The above copyright notice and this permission notice shall be        */
# * included in all copies or substantial portions of the Software.       */
# *                                                                       */
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
# * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
# * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
# * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
# * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
# * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
# * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
# *************************************************************************/

cmake_minimum_required(VERSION 3.0)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.12.7.tar.gz"
    SHA1 "b5ae065573ea2706ae1389a9048f1cabb2451f23"
)

project(spdlog VERSION 1.0.0)
include(CTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
endif()

add_library(spdlog INTERFACE)

option(SPDLOG_BUILD_EXAMPLES "Build examples" OFF)
option(SPDLOG_BUILD_TESTS "Build tests" OFF)

target_include_directories(
    spdlog
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(SPDLOG_BUILD_EXAMPLES)
  enable_testing()
  add_subdirectory(example)
endif()

if(SPDLOG_BUILD_TESTS)
    add_subdirectory(tests)
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

# Note: use 'targets_export_name'
configure_file("cmake/Config.cmake.in" "${project_config}" @ONLY)
configure_file("cmake/spdlog.pc.in" "${pkg_config}" @ONLY)

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
