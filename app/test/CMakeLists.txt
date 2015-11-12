cmake_minimum_required(VERSION 2.8.12)
project(app C)

add_executable(app main.c)

target_link_libraries(app LINK_PUBLIC platform_lib)
