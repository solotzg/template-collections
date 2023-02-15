add_executable(BenchMPSC bench_mpsc.cpp)
add_target_pch(pch.h BenchMPSC)
target_include_directories(BenchMPSC PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries (BenchMPSC fmt::fmt utils)
