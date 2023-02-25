# Linker
string(REGEX MATCHALL "[0-9]+" COMPILER_VERSION_LIST
             ${CMAKE_CXX_COMPILER_VERSION})
list(GET COMPILER_VERSION_LIST 0 COMPILER_VERSION_MAJOR)

# Example values: `lld-10`, `gold`.
option(LINKER_NAME "Linker name or full path")

find_program(LLD_PATH NAMES "lld${COMPILER_POSTFIX}" "lld")
find_program(GOLD_PATH NAMES "ld.gold" "gold")
if(OS_DARWIN)
  find_program(LD_PATH NAMES "ld")
endif()

if(NOT LINKER_NAME)
  if(OS_DARWIN AND COMPILER_APPLE_CLANG)
    set(LINKER_NAME "ld")
  elseif(LLD_PATH)
    set(LINKER_NAME "lld")
  elseif(GOLD_PATH)
    message(WARNING "Linking with gold is not recommended. Please use lld.")
    set(LINKER_NAME "gold")
  endif()
endif()

if(LINKER_NAME)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=${LINKER_NAME}")
  message(STATUS "Using linker: ${LINKER_NAME}")
else()
  message(STATUS "Using linker: <default>")
endif()

option(ENABLE_THINLTO "Clang-specific link time optimization" OFF)

if(ENABLE_THINLTO)
  # Link time optimization
  set(THINLTO_JOBS
      "0"
      CACHE STRING "ThinLTO compilation parallelism")
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -flto=thin -fvisibility=hidden -fvisibility-inlines-hidden -fsplit-lto-unit"
  )
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -flto=thin -fvisibility=hidden -fvisibility-inlines-hidden -fwhole-program-vtables -fsplit-lto-unit"
  )
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -flto=thin -flto-jobs=${THINLTO_JOBS} -fvisibility=hidden -fvisibility-inlines-hidden -fwhole-program-vtables -fsplit-lto-unit"
  )
  message(STATUS "Using `ThinLTO`, lto-jobs=${THINLTO_JOBS}")
endif()
