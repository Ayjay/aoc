cmake_minimum_required(VERSION 3.22)

project(aoc)

# if(NOT DEFINED ${VCPKG_ROOT})
#     message(FATAL_ERROR "vcpkg required - install and set VCPKG_ROOT")
# endif()
# set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(2022)
add_subdirectory(2023)