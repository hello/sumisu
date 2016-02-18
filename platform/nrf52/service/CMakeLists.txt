
file(GLOB service_src
    #IMU
    "${CMAKE_CURRENT_SOURCE_DIR}/os_imu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/os_imu_driver_mpu6500.c"
    "${sdk_location}/components/drivers_nrf/spi_master/nrf_drv_spi.c"
    )


list(APPEND platform_src ${service_src})
set(platform_src "${platform_src}" PARENT_SCOPE)
