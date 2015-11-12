cmake_minimum_required(VERSION 2.8.12)

file(GLOB platform_src
    "*.h"
    "*.c"
    )

add_library(platform_lib ${platform_src})
target_include_directories(platform_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

