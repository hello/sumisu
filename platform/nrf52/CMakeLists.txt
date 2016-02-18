cmake_minimum_required(VERSION 2.8.12)
project(platform C ASM)

set(sdk_version nRF52_SDK_0.9.2_dbc28c9)
set(softdevice_name s132_nrf52_1.0.0-3.alpha_softdevice)
set(sdk_location ${CMAKE_CURRENT_SOURCE_DIR}/sdk/${sdk_version})
set(softdevice_location ${sdk_location}/components/softdevice/s132)
set(softdevice_hex ${softdevice_location}/hex/${softdevice_name}.hex)

file(GLOB platform_src_asm 
    "${sdk_location}/components/toolchain/gcc/gcc_startup_nrf52.s"
    )
set_property(SOURCE ${platform_src_asm} PROPERTY LANGUAGE C)

file(GLOB platform_src
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${sdk_location}/components/toolchain/system_nrf52.c"
    "${sdk_location}/components/drivers_nrf/common/nrf_drv_common.c"
    "${sdk_location}/components/softdevice/common/softdevice_handler/softdevice_handler.c"
    "${sdk_location}/components/drivers_nrf/gpiote/nrf_drv_gpiote.c"
    )

include_directories(
    "${sdk_location}/components/toolchain/gcc"
    "${sdk_location}/components/softdevice/common/softdevice_handler"
    "${sdk_location}/components/softdevice/s132/headers"
    "${sdk_location}/components/toolchain"
    "${sdk_location}/components/toolchain/gcc"
    "${sdk_location}/components/device"
    "${sdk_location}/components/softdevice/s132/headers/nrf52"
    "${sdk_location}/components/drivers_nrf/common"
    "${sdk_location}/components/drivers_nrf/hal"
    "${sdk_location}/components/libraries/util"
    "${sdk_location}/components/libraries/scheduler"
    "${sdk_location}/components/drivers_nrf/gpiote"
    #todo separate below
    "${sdk_location}/components/ble/ble_dtm"
    #board specific defines
    "${sdk_location}/examples/bsp"
    #delay
    "${sdk_location}/components/drivers_nrf/delay"
    #freertos
    "${CMAKE_CURRENT_SOURCE_DIR}/freertos/source/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/freertos/portable/CMSIS/nrf52"
    "${CMAKE_CURRENT_SOURCE_DIR}/freertos/portable/GCC/nrf52"
    #clockd
    "${sdk_location}/components/drivers_nrf/clock"
    #ble
    "${sdk_location}/components/ble/common"
    "${sdk_location}/components/ble/ble_advertising"
    #app trace
    "${sdk_location}/components/libraries/trace"
    #pstorage
    "${sdk_location}/components/drivers_nrf/pstorage"
    #nordic uart service
    "${sdk_location}/components/ble/ble_services/ble_nus"
    #battery service
    "${sdk_location}/components/ble/ble_services/ble_bas"
    #device information
    "${sdk_location}/components/ble/ble_services/ble_dis"
    #segger
    "${sdk_location}/components/drivers_ext/segger_rtt"
    #uart
    "${sdk_location}/components/drivers_nrf/uart"
    "${sdk_location}/components/libraries/fifo"
    #spi master
    "${sdk_location}/components/drivers_nrf/spi_master"
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
add_to_platform(ble)
add_to_platform(freertos)
add_to_platform(service)

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

##############################################
# Convert hex of softdevice to binary
##############################################
add_custom_target(softdevice_bin
    COMMAND ${OBJCOPY} -I ihex -O binary ${softdevice_hex} ${EXECUTABLE_OUTPUT_PATH}/${softdevice_name}.bin)
add_dependencies(platform softdevice_bin)

##############################################
# Generate jlink flash output
##############################################
set(jlink_input ${CMAKE_CURRENT_SOURCE_DIR}/ld/nrf52_softdevice_app.jlink)
set(jlink_output ${EXECUTABLE_OUTPUT_PATH}/${BUILD_OUTPUT_NAME}.jlink)
add_custom_target(jlink_script
    COMMAND sed
    -e "s,%SOFTDEVICE,${EXECUTABLE_OUTPUT_PATH}/${softdevice_name}.bin,g"
    -e "s,%APP,${EXECUTABLE_OUTPUT_PATH}/${BUILD_OUTPUT_NAME}.bin,g"
    ${jlink_input} > ${jlink_output})
add_dependencies(platform jlink_script)

