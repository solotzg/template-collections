cmake_minimum_required(VERSION 3.21)

project(CPPTest)
set(CMAKE_CXX_STANDARD 17)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UC)
message( STATUS "Build type `${CMAKE_BUILD_TYPE_UC}`")

set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O3")

include (cmake/utils.cmake)
include (cmake/linker.cmake)
add_subdirectory (contrib)

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_library(utils ${PROJECT_SOURCE_DIR}/utils/utils.cpp)
add_target_pch("${PROJECT_SOURCE_DIR}/utils/utils.h" utils)
target_link_libraries (utils fmt::fmt)

add_executable(CPPTest main.cpp)
add_target_pch("${PROJECT_SOURCE_DIR}/utils/utils.h" CPPTest)
target_include_directories(CPPTest PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries (CPPTest fmt::fmt utils)

option(ENABLE_BENCHMARK "enable build benchmark" OFF)
if (ENABLE_BENCHMARK AND NOT CMAKE_BUILD_TYPE_UC STREQUAL "DEBUG")
    add_subdirectory (bench)
endif ()

print_flags ()
