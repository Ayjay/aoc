cmake_minimum_required(VERSION 3.22)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost 1.87 CONFIG REQUIRED ALL)

if(${USE_GUI})
    find_package(imgui REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
endif()

# if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#     set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# endif()

include(${PROJECT_SOURCE_DIR}/aoc_day.cmake)

add_executable(test_2024 test.cpp)
add_test(NAME Test2024 COMMAND test_2024)
function(aoc24_day year day)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/day${day}.cpp")
        day_available(${year} ${day} is_available)
        if(is_available)
            download_input(${year} ${day} is_file_ready)
            if(is_file_ready)
                pad_day_string(${day} padded_day)
                set(lib_name "libaoc${year}_${padded_day}")
                add_library(${lib_name} OBJECT day${day}.cpp)
                set_property(TARGET ${lib_name} PROPERTY CXX_STANDARD 23)
                target_compile_definitions(${lib_name} PUBLIC AOC_DAY=${day})
                target_link_libraries(${lib_name}
                    PRIVATE 
                        fmt::fmt
                        range-v3
                        ${Boost_LIBRARIES}
                )
                if(MSVC)
                    target_compile_options(${lib_name} PRIVATE "/Zc:__cplusplus")
                endif()

                target_include_directories(${lib_name}
                    PRIVATE
                        ${Boost_INCLUDE_DIRS}
                        ${CMAKE_SOURCE_DIR}/common
                )
                target_link_libraries(test_2024 PRIVATE ${lib_name})
                set(app_name "aoc${year}_${padded_day}")
                add_executable(${app_name} main.cpp)
                target_link_libraries(${app_name} PRIVATE ${lib_name})
            endif()
        endif()
    endif()
endfunction()

foreach(day RANGE 1 25)
    aoc24_day(2024 ${day})
endforeach()

target_link_libraries(test_2024 PRIVATE Boost::unit_test_framework)

