cmake_minimum_required(VERSION 3.21)

project(CppTest)
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

set(USE_SHARED_LIB OFF)

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

option(ENABLE_PYBIND_UTILS "build pybind utils" OFF)

if(ENABLE_PYBIND_UTILS)
  set(USE_SHARED_LIB ON)
endif()

if(USE_SHARED_LIB)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_subdirectory(contrib)
add_subdirectory(utils)

if(ENABLE_PYBIND_UTILS)
  message(STATUS "Enable pybind utils")
  add_subdirectory(pybind_utils)
endif()

add_executable(CppTest main.cpp)
add_target_pch("${PROJECT_SOURCE_DIR}/utils/pch.h" CppTest)
target_include_directories(CppTest PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(CppTest utils)

option(ENABLE_BENCHMARK "enable build benchmark" OFF)
if(ENABLE_BENCHMARK)
  add_subdirectory(bench)
endif()

if(CMAKE_BUILD_TYPE_UC STREQUAL "DEBUG")
  add_subdirectory(tests)
endif()

print_flags()
