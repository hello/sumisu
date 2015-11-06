add_library(platform_lib test.c)

target_include_directories(platform_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

