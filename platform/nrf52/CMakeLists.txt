cmake_minimum_required(VERSION 2.8.12)
project(platform C ASM)

set(sdk_version nRF52_SDK_0.9.2_dbc28c9)
set(sdk_location ${CMAKE_CURRENT_SOURCE_DIR}/sdk/${sdk_version})

file(GLOB platform_src_asm 
    "${sdk_location}/components/toolchain/gcc/gcc_startup_nrf52.s"
    )
set_property(SOURCE ${platform_src_asm} PROPERTY LANGUAGE C)

file(GLOB platform_src
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${sdk_location}/components/toolchain/system_nrf52.c"
    "${sdk_location}/components/drivers_nrf/common/nrf_drv_common.c"
    "${sdk_location}/components/softdevice/common/softdevice_handler/softdevice_handler.c"
    )

include_directories(
    "${sdk_location}/components/toolchain/gcc"
    "${sdk_location}/components/softdevice/common/softdevice_handler"
    "${sdk_location}/components/softdevice/s132/headers"
    "${sdk_location}/components/toolchain"
    "${sdk_location}/components/device"
    "${sdk_location}/components/softdevice/s132/headers/nrf52"
    "${sdk_location}/components/drivers_nrf/common"
    "${sdk_location}/components/drivers_nrf/hal"
    "${sdk_location}/components/libraries/fifo"
    "${sdk_location}/components/libraries/util"
    "${sdk_location}/components/libraries/timer"
    "${sdk_location}/components/libraries/scheduler"
    #todo separate below
    "${sdk_location}/components/ble/ble_dtm"
    #board specific defines
    "${sdk_location}/examples/bsp"
    #platform
    "${sdk_location}/components/drivers_nrf/uart"
    #delay
    "${sdk_location}/components/drivers_nrf/delay"
    #freertos
    "${CMAKE_CURRENT_SOURCE_DIR}/freertos/FreeRTOS/Source/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/freertos/FreeRTOS/Source/portable/GCC/nRF51_SD"
    #timer
    "${sdk_location}/components/drivers_nrf/timer"
    )

##############################################
# Platform Config
##############################################
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/hwconfig"
    "${CMAKE_CURRENT_SOURCE_DIR}/hwconfig/${TARGET_HARDWARE}"
    )

##############################################
# Add modules (modifies platform_src)
##############################################
macro(add_to_platform folder)
    include_directories(${folder})
    add_subdirectory(${folder})
endmacro(add_to_platform)

add_to_platform(io)
add_to_platform(misc)
add_to_platform(freertos)

##############################################
# Build Lib(Must take place after Adding modules)
##############################################
add_library(platform OBJECT ${platform_src} ${platform_src_asm})

##############################################
# Set Linker NRF52
##############################################
set(LINKER_FLAGS
    "-Xlinker"
    "-L ${CMAKE_CURRENT_SOURCE_DIR}/ld"
    "-Xlinker"
    "-Tnrf52_s132_app.ld"
    "-Xlinker"
    "-Map=${BUILD_OUTPUT_NAME}.map"
    )
