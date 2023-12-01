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

string(TIMESTAMP NOW "%Y%m%d")
function(aoc_day DAY)
    set(DAY_STRING "202312")
    string(LENGTH "${DAY}" DAY_LENGTH)
    if(${DAY_LENGTH} EQUAL 1)
        string(APPEND DAY_STRING "0")
    endif()
    string(APPEND DAY_STRING ${DAY})
    set(APP_NAME aoc23_${DAY})
    if(${NOW} GREATER_EQUAL ${DAY_STRING})
        add_executable(${APP_NAME} day${DAY}.cpp)
        set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 23)
        target_compile_definitions(${APP_NAME} PUBLIC AOC_DAY=${DAY})
        target_link_libraries(${APP_NAME}
            PRIVATE 
                fmt::fmt
                range-v3
                ${Boost_LIBRARIES}
        )
        target_include_directories(${APP_NAME}
            PRIVATE
                ${Boost_INCLUDE_DIRS}
        )
        file(DOWNLOAD 
            "https://adventofcode.com/2023/day/${DAY}/input"
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

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)