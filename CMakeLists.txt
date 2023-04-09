cmake_minimum_required(VERSION 3.21)

project(CPPTest)
set(CMAKE_CXX_STANDARD 20)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UC)
message(STATUS "Build type `${CMAKE_BUILD_TYPE_UC}`")

if(CMAKE_SYSTEM MATCHES "Linux")
  set(OS_LINUX 1)
elseif(CMAKE_SYSTEM MATCHES "FreeBSD")
  set(OS_FREEBSD 1)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(OS_DARWIN 1)
endif()

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O3")

include(cmake/version.cmake)
include(cmake/utils.cmake)
include(cmake/linker.cmake)

include(cmake/test_compiler.cmake)

if(OS_LINUX)
  option(
    USE_LIBCXX
    "Use libc++ and libc++abi instead of libstdc++ (only make sense on Linux with Clang)"
    ${HAVE_LIBCXX})
  if(USE_LIBCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++ -lc++abi")
  endif()
endif()

set(BUILD_INFO ${CMAKE_CURRENT_BINARY_DIR}/utils/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utils/version.h.in ${BUILD_INFO})
add_library(version ${CMAKE_CURRENT_SOURCE_DIR}/utils/build_info.cpp)
target_include_directories(version PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/utils)

add_subdirectory(contrib)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_library(utils ${PROJECT_SOURCE_DIR}/utils/utils.cpp)
add_target_pch("${PROJECT_SOURCE_DIR}/utils/pch.h" utils)
target_link_libraries(utils fmt::fmt version)

add_executable(CPPTest main.cpp)
add_target_pch("${PROJECT_SOURCE_DIR}/utils/pch.h" CPPTest)
target_include_directories(CPPTest PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(CPPTest utils)

option(ENABLE_BENCHMARK "enable build benchmark" OFF)
if(ENABLE_BENCHMARK AND NOT CMAKE_BUILD_TYPE_UC STREQUAL "DEBUG")
  add_subdirectory(bench)
endif()

if(CMAKE_BUILD_TYPE_UC STREQUAL "DEBUG")
  add_subdirectory(example)
endif()

print_flags()
