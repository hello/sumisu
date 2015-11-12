#/* vim: set ft=cmake: */

set(CMAKE_SYSTEM_NAME darwin)
set(TARGET_PLATFORM darwin)

##############################################
# Location
##############################################
set(GCC_PACKAGE gcc-arm-none-eabi-4_7-2013q3-20130916-mac)
set(GCC_URL https://launchpadlibrarian.net/151487551/${GCC_PACKAGE}.tar.bz2)
set(GCC_ROOT ${PROJECT_SOURCE_DIR}/tools/${GCC_PACKAGE}/bin)

##############################################
# Download and Setup
##############################################
#file(DOWNLOAD ${DOWNLOAD_SOURCE} ${GCC_ROOT} SHOWPROGRESS COMMENT "Downloading ${GCC_ROOT}")
#execute_process(COMMAND tar jxf ${GCC_ROOT} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

##############################################
# TOOLCHAIN
##############################################
set(CMAKE_CC_COMPILER ${GCC_ROOT}/arm-none-eabi-gcc)
