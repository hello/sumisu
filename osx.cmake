#/* vim: set ft=cmake: */

set(CMAKE_SYSTEM_NAME darwin)
set(TARGET_PLATFORM darwin)

##############################################
# Download and Setup
##############################################
#file(DOWNLOAD ${DOWNLOAD_SOURCE} ${GCC_ROOT} SHOWPROGRESS COMMENT "Downloading ${GCC_ROOT}")
#execute_process(COMMAND tar jxf ${GCC_ROOT} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

##############################################
# TOOLCHAIN(Use default)
##############################################
