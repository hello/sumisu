cmake_minimum_required(VERSION 2.8.12)
project(platform_common C ASM)

file(GLOB src
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    )

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    )

add_library(platform_common OBJECT ${src})
