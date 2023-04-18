add_executable(bench bench.cpp bench_mpsc.cpp bench_timer.cpp bench_lock.cpp bench_alloc.cpp)
add_target_pch(pch.h bench)
target_include_directories(bench PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(bench fmt::fmt utils)
