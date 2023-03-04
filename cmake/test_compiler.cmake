include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

cmake_push_check_state()

set(TEST_FLAG "-stdlib=libc++ -lc++ -lc++abi")

set(CMAKE_REQUIRED_FLAGS "${TEST_FLAG}")

check_cxx_source_compiles(
  "
    #include <iostream>
    int main() {
        std::cerr << std::endl;
        return 0;
    }
    "
  HAVE_LIBCXX)

cmake_pop_check_state()
