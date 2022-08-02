cmake_minimum_required(VERSION 3.21)

project(CPPTest)
set(CMAKE_CXX_STANDARD 17)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UC)
message(STATUS "Build type `${CMAKE_BUILD_TYPE_UC}`")

find_program (LLD_PATH NAMES "lld")
find_program (GOLD_PATH NAMES "gold")

if (NOT LINKER_NAME AND NOT APPLE)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND LLD_PATH)
        set (LINKER_NAME "lld")
    elseif (GOLD_PATH)
        set (LINKER_NAME "gold")
    endif ()
endif ()

if (LINKER_NAME)
    message(STATUS "Using linker: ${LINKER_NAME} (selected from: LLD_PATH=${LLD_PATH}; GOLD_PATH=${GOLD_PATH};)")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=${LINKER_NAME}")
endif ()

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_executable(CPPTest main.cpp ${PROJECT_SOURCE_DIR}/utils/work.cpp)

function(add_target_pch context target)
    if (ENABLE_PCH)
        message(STATUS "Add PCH `${context}` for target `${target}`")
        target_precompile_headers(${target} PRIVATE ${context})
    endif ()
    if(${ARGC} GREATER 2)
        add_target_pch(${context} ${ARGN})
    endif()
endfunction()

option(ENABLE_TIME_TRACES "enable clang feature time traces" OFF)

if(ENABLE_TIME_TRACES)
    set(CLANG_TIME_TRACES_FLAGS "-ftime-trace")
    message(STATUS "Using clang time traces flag `${CLANG_TIME_TRACES_FLAGS}`")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CLANG_TIME_TRACES_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_TIME_TRACES_FLAGS}")
endif()

option(ENABLE_THINLTO "Clang-specific link time optimization" OFF)

if (ENABLE_THINLTO)
    # Link time optimization
    set (THINLTO_JOBS "0" CACHE STRING "ThinLTO compilation parallelism")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto=thin -fvisibility=hidden -fvisibility-inlines-hidden -fsplit-lto-unit")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=thin -fvisibility=hidden -fvisibility-inlines-hidden -fwhole-program-vtables -fsplit-lto-unit")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto=thin -flto-jobs=${THINLTO_JOBS} -fvisibility=hidden -fvisibility-inlines-hidden -fwhole-program-vtables -fsplit-lto-unit")
    message (STATUS "Using `ThinLTO`, lto-jobs=${THINLTO_JOBS}")
endif ()

option (USE_CCACHE "Set to OFF to disable ccache" OFF)
option (ENABLE_PCH "Enable `Precompiled header`" ON)
if (USE_CCACHE)
    find_program (CCACHE_FOUND ccache)
endif ()
if (USE_CCACHE AND CCACHE_FOUND AND NOT CMAKE_CXX_COMPILER_LAUNCHER MATCHES "ccache" AND NOT CMAKE_CXX_COMPILER MATCHES "ccache")
    execute_process (COMMAND ${CCACHE_FOUND} "-V" OUTPUT_VARIABLE CCACHE_VERSION)
    execute_process (COMMAND ${CCACHE_FOUND} "-p" OUTPUT_VARIABLE CCACHE_CONFIG)
    string (REGEX REPLACE "ccache version ([0-9\\.]+).*" "\\1" CCACHE_VERSION ${CCACHE_VERSION})
    message (STATUS "Using ccache: ${CCACHE_FOUND}, version ${CCACHE_VERSION}")
    message (STATUS "Show ccache config:")
    message ("${CCACHE_CONFIG}")
    set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
    set_property (GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_FOUND})

    if (ENABLE_PCH)
        execute_process (COMMAND ${CCACHE_FOUND} --get-config sloppiness OUTPUT_VARIABLE _CCACHE_SLOPPINESS OUTPUT_STRIP_TRAILING_WHITESPACE)
        string (FIND "${_CCACHE_SLOPPINESS}" "pch_defines" _CCACHE_SLOPPINESS_RES)
        if (NOT _CCACHE_SLOPPINESS_RES STREQUAL "-1")
            string (FIND "${_CCACHE_SLOPPINESS}" "time_macros" _CCACHE_SLOPPINESS_RES)
        endif ()

        if (_CCACHE_SLOPPINESS_RES STREQUAL "-1")
            message(WARNING "`Precompiled header` won't be cached by ccache, sloppiness = `${CCACHE_SLOPPINESS}`,please execute `ccache -o sloppiness=pch_defines,time_macros`")
            set (ENABLE_PCH FALSE CACHE BOOL "" FORCE)
        endif ()
    endif ()
else ()
    message (STATUS "Not using ccache ${CCACHE_FOUND}, USE_CCACHE=${USE_CCACHE}")
endif ()

add_target_pch("${PROJECT_SOURCE_DIR}/utils/work.h" CPPTest)

option (ARCHNATIVE "Enable `-march=native` compiler flag" OFF)
if (ARCHNATIVE)
    message(STATUS "Using `-march=native` compiler flag")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif ()

target_include_directories(CPPTest PUBLIC ${PROJECT_SOURCE_DIR})

option (TEST_LLVM_COVERAGE "Enables flags for test coverage" OFF)
if (TEST_LLVM_COVERAGE)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
endif ()
