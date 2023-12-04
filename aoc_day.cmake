cmake_minimum_required(VERSION 3.22)

function(aoc_day YEAR DAY)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/day${DAY}.cpp")
        string(LENGTH "${DAY}" DAY_LENGTH)
        if(${DAY_LENGTH} EQUAL 1)
            set(FORMATTED_DAY "0${DAY}")
        else()
            set(FORMATTED_DAY "${DAY}")
        endif()
        set(DAY_STRING "${YEAR}12${FORMATTED_DAY}")
        string(TIMESTAMP NOW "%Y%m%d")
        if(${NOW} GREATER_EQUAL ${DAY_STRING})
            set(INPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/input${DAY}.txt")
            set(HAS_INPUT false)
            if(EXISTS ${INPUT_FILE})
                set(HAS_INPUT true)
            else()
                file(DOWNLOAD 
                    "https://adventofcode.com/${YEAR}/day/${DAY}/input"
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
                    set(HAS_INPUT true)
                else()
                    message(WARNING "Error occurred during download: ${ERROR_MESSAGE}")
                endif()
            endif()
            if(${HAS_INPUT})
                set(APP_NAME aoc${YEAR}_${FORMATTED_DAY})
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
            endif()
        endif()
    endif()
endfunction()
