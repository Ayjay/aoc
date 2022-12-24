cmake_minimum_required(VERSION 3.17)

project(aoc)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED)

set(SESSION_TOKEN "53616c7465645f5f07bc518026bb66d98c2c822ed5fbcb6ba48451e15e7fd0a46d3a67116100a3c84bc23953812ffeb2ed74da859da67d3ec637bf578fb49b06")

function(aoc_day DAY)
    add_executable(day${DAY} day${DAY}.cpp)
    set_property(TARGET day${DAY} PROPERTY CXX_STANDARD 20)
    target_compile_definitions(day${DAY} PUBLIC AOC_DAY=${DAY})
    target_link_libraries(day${DAY}
        PRIVATE 
            fmt::fmt
            range-v3
            ${Boost_LIBRARIES}
    )
    target_include_directories(day${DAY}
        PRIVATE
            ${Boost_INCLUDE_DIRS}
    )
    file(DOWNLOAD 
        "https://adventofcode.com/2022/day/${DAY}/input"
        "${CMAKE_CURRENT_BINARY_DIR}/input${DAY}.txt"
        HTTPHEADER "Cookie: session=${SESSION_TOKEN}"
    )
endfunction()

foreach(day RANGE 1 11)
    aoc_day(${day})
endforeach()
