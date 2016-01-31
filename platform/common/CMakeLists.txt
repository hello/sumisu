cmake_minimum_required(VERSION 2.8.12)
project(platform_common C ASM)

file(GLOB src
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    )

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${sdk_location}/components/drivers_ext/segger_rtt"
    )

add_library(platform_common OBJECT ${src})
