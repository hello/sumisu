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
    "*.c"
    )

add_library(platform OBJECT ${platform_src} ${platform_src_asm})

include_directories(#platform PUBLIC
    "${sdk_location}/components/toolchain/gcc"
    "${sdk_location}/components/softdevice/s132/headers"
    "${sdk_location}/examples/bsp"
    "${sdk_location}/components/toolchain"
    "${sdk_location}/components/device"
    "${sdk_location}/components/softdevice/s132/headers/nrf52"
    "${sdk_location}/components/drivers_nf/hal"
    )

set(LINKER_FLAGS
    "-Xlinker"
    "-L ${CMAKE_CURRENT_SOURCE_DIR}/ld"
    "-Tnrf52_xxaa.ld"
    )
