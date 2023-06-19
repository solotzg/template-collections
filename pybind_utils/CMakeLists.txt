find_program(PYTHON3_FOUND python3 REQUIRED)

execute_process(
  COMMAND ${PYTHON3_FOUND} -m pybind11 --cmakedir
  OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PYBIND11_CMAKE_DIR})

find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(pybind_utils src/lib.cpp)
add_target_pch(${PROJECT_SOURCE_DIR}/pybind_utils/pch.h pybind_utils)
target_link_libraries(pybind_utils PRIVATE utils fmt::fmt)
target_include_directories(pybind_utils PRIVATE ${PROJECT_SOURCE_DIR})
