add_executable(example test-coroutine.cpp)
add_target_pch("${PROJECT_SOURCE_DIR}/example/pch.h" example)
target_include_directories(example PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(example fmt::fmt utils)
