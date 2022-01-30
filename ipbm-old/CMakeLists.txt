cmake_minimum_required(VERSION 3.16)
project(reconf_switch)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_BUILD_TYPE STREQUAL "")
    message("CMAKE_BUILD_TYPE not specified. Default to Release mode.")
    set(CMAKE_BUILD_TYPE Release)
endif ()

message("Using ${CMAKE_BUILD_TYPE} mode.")

find_package(gflags REQUIRED)
find_package(yaml-cpp REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

enable_testing()
add_subdirectory(src)
