file(GLOB io_src
    "${sdk_location}/components/drivers_nrf/uart/nrf_drv_uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/io.c"
    )


list(APPEND platform_src ${io_src})
set(platform_src "${platform_src}" PARENT_SCOPE)
