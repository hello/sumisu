file(GLOB src
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${sdk_location}/components/ble/common/*.c"
    "${sdk_location}/components/ble/ble_advertising/*.c"
    "${sdk_location}/components/drivers_nrf/pstorage/pstorage.c"
    "${sdk_location}/components/ble/ble_services/ble_nus/ble_nus.c"
    )


list(APPEND platform_src ${src})
set(platform_src "${platform_src}" PARENT_SCOPE)
