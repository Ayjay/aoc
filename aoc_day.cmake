cmake_minimum_required(VERSION 3.22)

function(pad_day_string day output)
    string(LENGTH "${day}" day_length)
    if(${day_length} EQUAL 1)
        set(prefix "0")
    endif()
    set(${output} ${prefix}${day} PARENT_SCOPE)
endfunction()

function(day_available year day is_available)
    pad_day_string(${day} padded_day)
    set(day_string "${year}12${padded_day}")
    string(TIMESTAMP now "%Y%m%d")
    if(${now} GREATER_EQUAL ${day_string})
        set(${is_available} true PARENT_SCOPE)
    endif()
endfunction()

function(download_input year day FILE_READY)
    set(INPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/input${day}.txt")
    set(HAS_INPUT false)
    if(EXISTS ${INPUT_FILE})
        file(SIZE ${INPUT_FILE} INPUT_FILE_SIZE)
        if(${INPUT_FILE_SIZE} GREATER 0)
            set(HAS_INPUT true)
        endif()
    endif()

    if(NOT ${HAS_INPUT})
        file(DOWNLOAD 
            "https://adventofcode.com/${year}/day/${day}/input"
            "${CMAKE_CURRENT_BINARY_DIR}/input${day}.txt"
            HTTPHEADER "Cookie: session=${AOC_SESSION_TOKEN}"
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
    set(${FILE_READY} ${HAS_INPUT} PARENT_SCOPE)
endfunction()

function(add_day year day)
    pad_day_string(${day} padded_day)
    set(app_name aoc${year}_${padded_day})
    add_executable(${app_name} day${day}.cpp)
    set_property(TARGET ${app_name} PROPERTY CXX_STANDARD 23)
    target_compile_definitions(${app_name} PUBLIC AOC_DAY=${day})
    target_link_libraries(${app_name}
        PRIVATE 
            fmt::fmt
            range-v3
            ${Boost_LIBRARIES}
    )
    if(MSVC)
        target_compile_options(${app_name} PRIVATE "/Zc:__cplusplus")
    endif()

    if(${USE_GUI} AND "${imgui_FOUND}")
        target_compile_definitions(${app_name} PUBLIC USE_GUI=1)
        target_include_directories(${app_name} PRIVATE ${OPENGL_INCLUDE_DIRS})
        target_link_libraries(${app_name} PRIVATE
            ${OPENGL_LIBRARIES}
            imgui::imgui
            glfw
        )
    endif()

    target_include_directories(${app_name}
        PRIVATE
            ${Boost_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/common
    )
endfunction()

function(aoc_day year day)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/day${day}.cpp")
        day_available(${year} ${day} is_available)
        if(is_available)
            download_input(${year} ${day} is_file_ready)
            if(is_file_ready)
                add_day(${year} ${day})
            endif()
        endif()
    endif()
endfunction()
