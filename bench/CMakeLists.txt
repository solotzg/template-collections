set(BENCH_SRC bench.cpp)
file(GLOB _BENCH_SRC "src/bench-*.cpp")
set(BENCH_SRC ${BENCH_SRC} ${_BENCH_SRC})
message(STATUS "Using benchmark source files: ${BENCH_SRC}")

add_executable(bench ${BENCH_SRC})
add_target_pch(pch.h bench)
target_include_directories(bench PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(bench fmt::fmt utils)
