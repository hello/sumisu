
file(GLOB service_src
    "${CMAKE_CURRENT_SOURCE_DIR}/os_imu.c"
    )


list(APPEND platform_src ${service_src})
set(platform_src "${platform_src}" PARENT_SCOPE)
