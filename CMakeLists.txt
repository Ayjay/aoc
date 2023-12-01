cmake_minimum_required(VERSION 3.23)

project(aoc)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(2022)
add_subdirectory(2023)