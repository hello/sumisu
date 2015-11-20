cmake_minimum_required(VERSION 2.8.12)
project(app C)

file(GLOB app_src
    "${TARGET_APP}/*.c"
    )

add_library(app OBJECT ${app_src})
add_dependencies(app platform)
