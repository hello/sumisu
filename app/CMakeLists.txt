cmake_minimum_required(VERSION 2.8.12)
project(app C)

IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_APP}/")
    message(FATAL_ERROR "App folder ${TARGET_APP} does not exist.")
ENDIF()

file(GLOB app_src
    "${TARGET_APP}/*.c"
    )

add_library(app OBJECT ${app_src})
add_dependencies(app platform)
