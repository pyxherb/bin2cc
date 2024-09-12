cmake_minimum_required(VERSION 3.13)
project(file2c VERSION 0.1.0)

add_subdirectory("file2c")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/file2cConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/file2c)
