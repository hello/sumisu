
file(GLOB io_src
    #rtt
    "${CMAKE_CURRENT_SOURCE_DIR}/io_rtt.c"
    "${sdk_location}/components/drivers_ext/segger_rtt/SEGGER_RTT.h"
    "${sdk_location}/components/drivers_ext/segger_rtt/SEGGER_RTT_Conf.h"
    "${sdk_location}/components/drivers_ext/segger_rtt/RTT_Syscalls_GCC.c"
    "${sdk_location}/components/drivers_ext/segger_rtt/SEGGER_RTT_printf.c"
    "${sdk_location}/components/drivers_ext/segger_rtt/SEGGER_RTT.c"
    #uart
    #"${CMAKE_CURRENT_SOURCE_DIR}/io_uart.c"
    #"${sdk_location}/components/drivers_nrf/uart/nrf_drv_uart.c"
    #"${sdk_location}/components/libraries/fifo/app_fifo.c"

    )


list(APPEND platform_src ${io_src})
set(platform_src "${platform_src}" PARENT_SCOPE)
