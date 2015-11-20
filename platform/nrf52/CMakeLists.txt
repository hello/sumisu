cmake_minimum_required(VERSION 2.8.12)
project(platform C ASM)

set(sdk_version nRF52_SDK_0.9.2_dbc28c9)
set(sdk_location ${CMAKE_CURRENT_SOURCE_DIR}/sdk/${sdk_version})

file(GLOB platform_src_asm 
    "${sdk_location}/components/toolchain/gcc/gcc_startup_nrf52.s"
    )
set_property(SOURCE ${platform_src_asm} PROPERTY LANGUAGE C)

file(GLOB platform_src
    "${sdk_location}/components/toolchain/system_nrf52.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${sdk_location}/components/drivers_nrf/common/nrf_drv_common.c"
    #todo separate below
    "${sdk_location}/components/ble/ble_dtm/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/misc/*.c"
    #uart driver
    "${sdk_location}/components/drivers_nrf/uart/nrf_drv_uart.c"
    )

add_library(platform OBJECT ${platform_src} ${platform_src_asm})

include_directories(#platform PUBLIC
    "${sdk_location}/components/toolchain/gcc"
    "${sdk_location}/components/softdevice/s132/headers"
    "${sdk_location}/components/toolchain"
    "${sdk_location}/components/device"
    "${sdk_location}/components/softdevice/s132/headers/nrf52"
    "${sdk_location}/components/drivers_nrf/common"
    "${sdk_location}/components/drivers_nrf/hal"
    "${sdk_location}/components/libraries/util"
    #todo separate below
    "${sdk_location}/components/ble/ble_dtm"
    #board specific defines
    "${sdk_location}/examples/bsp"
    #uart driver
    "${sdk_location}/components/drivers_nrf/uart"
    "${sdk_location}/components/drivers_nrf/uart"
    )

#platform specific config
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/hwconfig"
    "${CMAKE_CURRENT_SOURCE_DIR}/hwconfig/${TARGET_HARDWARE}"
    )

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
