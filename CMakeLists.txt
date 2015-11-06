# CMake Script for sumisu


project(sumisu)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}/os")

add_executable(test ${PROJECT_SOURCE_DIR}/apps/test/main.c)


