set(TESTS_SRC tests.cpp)
file(GLOB _TESTS_SRC "src/test-*.cpp")
set(TESTS_SRC ${TESTS_SRC} ${_TESTS_SRC})
message(STATUS "Using tests source files: ${TESTS_SRC}")

add_executable(tests ${TESTS_SRC})
add_target_pch("${PROJECT_SOURCE_DIR}/tests/pch.h" tests)
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(tests fmt::fmt utils)
