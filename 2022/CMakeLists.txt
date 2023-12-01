cmake_minimum_required(VERSION 3.23)

project(aoc)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(SESSION_TOKEN "53616c7465645f5fbb99c62c95ced6945f3d8364383aeec73c6609d406368f73b4b25c9c79aa27bc123c8fc1163ca218bc0bf9ea6c5772ca80c471df5e854bbf")

add_library(pch STATIC aoc.cpp)
set_property(TARGET pch PROPERTY CXX_STANDARD 23)
target_precompile_headers(pch PUBLIC "aoc.h")
target_link_libraries(pch
    PRIVATE 
        fmt::fmt
        range-v3
        ${Boost_LIBRARIES}
)
target_include_directories(pch
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

function(aoc_day DAY)
    add_executable(day${DAY} day${DAY}.cpp)
    set_property(TARGET day${DAY} PROPERTY CXX_STANDARD 23)
    target_compile_definitions(day${DAY} PUBLIC AOC_DAY=${DAY})
    target_precompile_headers(day${DAY} REUSE_FROM pch)
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
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/input${DAY}.txt")
        file(DOWNLOAD 
            "https://adventofcode.com/2022/day/${DAY}/input"
            "${CMAKE_CURRENT_BINARY_DIR}/input${DAY}.txt"
            HTTPHEADER "Cookie: session=${SESSION_TOKEN}"
            STATUS DOWNLOAD_STATUS
        )
        # Separate the returned status code, and error message.
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
        # Check if download was successful.
        if(${STATUS_CODE} EQUAL 0)
            message(STATUS "Download completed successfully!")
        else()
            # Exit CMake if the download failed, printing the error message.
            message(FATAL_ERROR "Error occurred during download: ${ERROR_MESSAGE}")
        endif()
    endif()
endfunction()

foreach(day RANGE 1 25)
    aoc_day(${day})
endforeach()

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)