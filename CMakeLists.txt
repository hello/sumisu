# CMake Script for sumisu


project(sumisu)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_DIR})

set(TARGET_APP test)
set(TARGET_PLATFORM darwin)

include_directories("${PROJECT_SOURCE_DIR}/include/*")

set(SOURCES ${PROJECT_SOURCE_DIR}/app/${TARGET_APP}/main.c)

add_executable(test ${SOURCES})
install(TARGETS test DESTINATION bin)


