cmake_minimum_required(VERSION 3.21)

project(CPPTest)
set(CMAKE_CXX_STANDARD 17)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UC)
message( STATUS "Build type `${CMAKE_BUILD_TYPE_UC}`")

include (cmake/utils.cmake)
include (cmake/linker.cmake)
add_subdirectory (contrib)

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_executable(CPPTest main.cpp ${PROJECT_SOURCE_DIR}/utils/work.cpp)

add_target_pch("${PROJECT_SOURCE_DIR}/utils/work.h" CPPTest)

target_include_directories(CPPTest PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries (CPPTest fmt::fmt)

print_flags ()
