
file(GLOB io_src
    "${CMAKE_CURRENT_SOURCE_DIR}/RTT_Syscalls_GCC.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/SEGGER_RTT_printf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/SEGGER_RTT.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/io_rtt.c"
    )


list(APPEND platform_src ${io_src})
set(platform_src "${platform_src}" PARENT_SCOPE)
