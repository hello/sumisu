#/* vim: set ft=cmake: */

set(TARGET_PLATFORM nrf52)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
##############################################
# Location
##############################################

set(GCC_PACKAGE gcc-arm-none-eabi-4_7-2013q3)
#set(GCC_URL https://launchpadlibrarian.net/151487551/${GCC_PACKAGE}-mac.tar.bz2)
set(GCC_ROOT ${PROJECT_SOURCE_DIR}/tools/${GCC_PACKAGE})

##############################################
# Download and Setup
##############################################
#file(DOWNLOAD ${DOWNLOAD_SOURCE} ${GCC_ROOT} SHOWPROGRESS COMMENT "Downloading ${GCC_ROOT}")
#execute_process(COMMAND tar jxf ${GCC_ROOT} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

##############################################
# TOOLCHAIN
##############################################
set(CMAKE_C_COMPILER ${GCC_ROOT}/bin/arm-none-eabi-gcc)
set(CMAKE_LINKER ${GCC_ROOT}/bin/arm-none-eabi-ld)

set(CMAKE_INSTALL_PREFIX ${GCC_ROOT}/bin)

message(STATUS "Using toolchain: ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_FIND_ROOT_PATH ${CMAKE_INSTALL_PREFIX})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_LIBRARY ONLY)

set(CMAKE_FIND_ROOT_PATH_INCLUDE ONLY)

set(CMAKE_C_FLAGS
    "-mthumb -mcpu=cortex-m4 -mabi=aapcs"
    "--specs=nano.specs -lc -lnosys"
    )

string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")

set(BUILD_SHARED_LIBS OFF)
