add_subdirectory(fmt)

option(ENABLE_ASYNC_SIMPLE "Enable async_simple" OFF)
if(ENABLE_ASYNC_SIMPLE)
  set(ASYNC_SIMPLE_ENABLE_TESTS OFF)
  add_subdirectory(async_simple)
endif()
