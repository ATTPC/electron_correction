cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE "Release")
    message("Building release")
else()
    message("Building debug")
endif()

project("electron_correction")

find_package(ROOT REQUIRED)
find_package(Garfield REQUIRED)

set(ECORR_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_subdirectory(vendor/nlohmann_json)
add_subdirectory(src)