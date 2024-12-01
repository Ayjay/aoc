cmake_minimum_required(VERSION 3.22)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost CONFIG REQUIRED ALL)

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

foreach(day RANGE 1 25)
    aoc_day(2023 ${day})
endforeach()

add_executable(test_2023 tests.cpp day12.cpp)
target_link_libraries(test_2023 PRIVATE Boost::unit_test_framework)
#target_include_directories(test_2023 PRIVATE ${Boost_INCLUDE_DIRS})
