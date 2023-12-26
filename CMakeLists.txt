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

set(SESSION_TOKEN "53616c7465645f5f2625ab79c5db5f5fc69e532e14ddd5f1b7990edd834aaed5434738e521351bc81a4c68258b72298b4043224c9dd50dc08b1c024bb2006c91")
add_subdirectory(2022)
add_subdirectory(2023)