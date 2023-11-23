set(BENCH_SRC bench.cpp)
set(BENCH_SRC ${BENCH_SRC} bench_mpsc.cpp bench_timer.cpp bench_lock.cpp
              bench_alloc.cpp bench_async_timepoint.cpp)

if(OS_LINUX)
  set(BENCH_SRC ${BENCH_SRC} bench_async_log.cpp)
endif()

add_executable(bench ${BENCH_SRC})
add_target_pch(pch.h bench)
target_include_directories(bench PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(bench fmt::fmt utils)
