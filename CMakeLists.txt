cmake_minimum_required(VERSION 3.22)

option(USE_GUI "Build with GUI" OFF)
if(${USE_GUI})
    list(APPEND VCPKG_MANIFEST_FEATURES "gui")
endif()

project(aoc)

# if(NOT DEFINED ${VCPKG_ROOT})
#     message(FATAL_ERROR "vcpkg required - install and set VCPKG_ROOT")
# endif()
# set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(AOC_SESSION_TOKEN $ENV{AOC_SESSION_TOKEN} CACHE STRING "AoC session token")
if(NOT AOC_SESSION_TOKEN)
    message(FATAL_ERROR "Missing AOC session token")
endif()

add_subdirectory(2022)
add_subdirectory(2023)