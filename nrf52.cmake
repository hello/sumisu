#/* vim: set ft=cmake: */

set(TARGET_PLATFORM nrf52)
#todo set this to be configurable
set(TARGET_HARDWARE BOARD_PCA10040)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
##############################################
# Location TODO move to parent directory
##############################################

set(GCC_PACKAGE gcc-arm-none-eabi-4_7-2013q3)
#set(GCC_URL https://launchpadlibrarian.net/151487551/${GCC_PACKAGE}-mac.tar.bz2)
set(GCC_ROOT ${PROJECT_SOURCE_DIR}/tools/${GCC_PACKAGE})

##############################################
# Download and Setup TODO move to tools directory
##############################################
#file(DOWNLOAD ${DOWNLOAD_SOURCE} ${GCC_ROOT} SHOWPROGRESS COMMENT "Downloading ${GCC_ROOT}")
#execute_process(COMMAND tar jxf ${GCC_ROOT} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

##############################################
# TOOLCHAIN
##############################################
set(CMAKE_INSTALL_PREFIX ${GCC_ROOT}/bin)

message(STATUS "Using toolchain: ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_C_COMPILER ${CMAKE_INSTALL_PREFIX}/arm-none-eabi-gcc)
set(OBJCOPY ${GCC_ROOT}/arm-none-eabi/bin/objcopy)
set(READELF ${CMAKE_INSTALL_PREFIX}/arm-none-eabi-readelf)

##############################################
# Compile Options
##############################################
set(CMAKE_C_FLAGS
    "-mthumb -mcpu=cortex-m4 -mabi=aapcs"
    "-mfloat-abi=hard -mfpu=fpv4-sp-d16"
    "--specs=nano.specs -lc -lnosys"
    "-DNRF52"
    "-DS132 -DSOFTDEVICE_PRESENT"
    "-DBSP_DEFINES_ONLY"
    "-DBLE_STACK_SUPPORT_REQD"
    "-D${TARGET_HARDWARE}"
    #"-x assembler-with-cpp"
    )
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
