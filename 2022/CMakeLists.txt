cmake_minimum_required(VERSION 3.23)

project(aoc)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()

set(SESSION_TOKEN "53616c7465645f5fbb99c62c95ced6945f3d8364383aeec73c6609d406368f73b4b25c9c79aa27bc123c8fc1163ca218bc0bf9ea6c5772ca80c471df5e854bbf")
include(${CMAKE_CURRENT_SOURCE_DIR}/../aoc_day.cmake)

foreach(day RANGE 1 25)
    aoc_day(2022 ${day})
endforeach()
