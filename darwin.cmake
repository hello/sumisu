#/* vim: set ft=cmake: */

set(TARGET_PLATFORM darwin)
set(CMAKE_SYSTEM_NAME Generic)

##############################################
# TOOLCHAIN(Use default)
##############################################

#message(STATUS "Using toolchain: GCC")
set(CMAKE_C_COMPILER /usr/bin/clang)

set(CMAKE_C_FLAGS
    "-DBSP_DEFINES_ONLY"
    "-std=gnu99"
    "-m32"
    )
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
