cmake_minimum_required(VERSION 3.22)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost CONFIG REQUIRED)

# if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#     set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# endif()

include(${PROJECT_SOURCE_DIR}/aoc_day.cmake)

foreach(day RANGE 1 25)
    aoc_day(2022 ${day})
endforeach()
