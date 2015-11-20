cmake_minimum_required(VERSION 2.8.12)
project(platform C)

file(GLOB platform_src
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    )

add_library(platform OBJECT ${platform_src})
