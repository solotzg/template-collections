set(BUILD_INFO ${CMAKE_CURRENT_BINARY_DIR}/utils/version.h)
configure_file(${PROJECT_SOURCE_DIR}/utils/version.h.in ${BUILD_INFO})
add_library(version ${PROJECT_SOURCE_DIR}/utils/build_info.cpp)
target_include_directories(version PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/utils)

add_library(utils ${PROJECT_SOURCE_DIR}/utils/utils.cpp)
add_target_pch("${PROJECT_SOURCE_DIR}/utils/pch.h" utils)
target_link_libraries(utils fmt::fmt version)
