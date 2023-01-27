# OS
if (CMAKE_SYSTEM MATCHES "Linux")
    set (OS_LINUX 1)
elseif (CMAKE_SYSTEM MATCHES "FreeBSD")
    set (OS_FREEBSD 1)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set (OS_DARWIN 1)
endif ()

# Arch
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64.*|AARCH64.*|arm64.*|ARM64.*)")
    set (ARCH_AARCH64 1)
endif ()
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(amd64.*|AMD64.*|x86_64.*|X86_64.*)")
    set (ARCH_AMD64 1)
endif ()
if (ARCH_AARCH64 OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set (ARCH_ARM 1)
endif ()

# Time trace
option(ENABLE_TIME_TRACES "enable clang feature time traces" OFF)

if(ENABLE_TIME_TRACES)
    set(CLANG_TIME_TRACES_FLAGS "-ftime-trace")
    message(STATUS "Using clang time traces flag `${CLANG_TIME_TRACES_FLAGS}`")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CLANG_TIME_TRACES_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_TIME_TRACES_FLAGS}")
endif()

# PCH
function(add_target_pch context target)
    if (ENABLE_PCH)
        message(STATUS "Add PCH `${context}` for target `${target}`")
        target_precompile_headers(${target} PRIVATE ${context})
    endif ()
    if(${ARGC} GREATER 2)
        add_target_pch(${context} ${ARGN})
    endif()
endfunction()

option (ENABLE_PCH "Enable `Precompiled header`" ON)

# CCACHE
option (USE_CCACHE "Set to OFF to disable ccache" OFF)
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

# Use arch native
option (ARCHNATIVE "Enable `-march=native` compiler flag" OFF)
if (ARCHNATIVE AND NOT OS_DARWIN)
    message(STATUS "Using `-march=native` compiler flag")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif ()

# Code coverage
option (TEST_LLVM_COVERAGE "Enables flags for test coverage" OFF)
if (TEST_LLVM_COVERAGE)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
endif ()

# Other
function (print_flags)
    set (FULL_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
    set (FULL_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
    set (FULL_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
    message (STATUS "Compiler C   = ${CMAKE_C_COMPILER} ${FULL_C_FLAGS}")
    message (STATUS "Compiler CXX = ${CMAKE_CXX_COMPILER} ${FULL_CXX_FLAGS}")
    message (STATUS "Linker Flags = ${FULL_EXE_LINKER_FLAGS}")
endfunction ()
