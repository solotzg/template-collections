# OS
if (CMAKE_SYSTEM MATCHES "Linux")
    set (OS_LINUX 1)
elseif (CMAKE_SYSTEM MATCHES "FreeBSD")
    set (OS_FREEBSD 1)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set (OS_DARWIN 1)
endif ()

# Linker
string (REGEX MATCHALL "[0-9]+" COMPILER_VERSION_LIST ${CMAKE_CXX_COMPILER_VERSION})
list (GET COMPILER_VERSION_LIST 0 COMPILER_VERSION_MAJOR)

# Example values: `lld-10`, `gold`.
option (LINKER_NAME "Linker name or full path")

find_program (LLD_PATH NAMES "lld${COMPILER_POSTFIX}" "lld")
find_program (GOLD_PATH NAMES "ld.gold" "gold")
if (OS_DARWIN)
    find_program (LD_PATH NAMES "ld")
endif ()

if (NOT LINKER_NAME)
    if (OS_DARWIN AND COMPILER_APPLE_CLANG)
        set (LINKER_NAME "ld")
    elseif (LLD_PATH)
        set (LINKER_NAME "lld")
    elseif (GOLD_PATH)
        message (WARNING "Linking with gold is not recommended. Please use lld.")
        set (LINKER_NAME "gold")
    endif ()
endif ()

if (LINKER_NAME)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=${LINKER_NAME}")
    message(STATUS "Using linker: ${LINKER_NAME}")
else()
    message(STATUS "Using linker: <default>")
endif()

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

# Other
function (print_flags)
    set (FULL_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
    set (FULL_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
    set (FULL_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
    message (STATUS "Compiler C   = ${CMAKE_C_COMPILER} ${FULL_C_FLAGS}")
    message (STATUS "Compiler CXX = ${CMAKE_CXX_COMPILER} ${FULL_CXX_FLAGS}")
    message (STATUS "Linker Flags = ${FULL_EXE_LINKER_FLAGS}")
endfunction ()
