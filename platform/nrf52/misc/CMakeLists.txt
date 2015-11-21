
file(GLOB src
    "${sdk_location}/components/ble/ble_dtm/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    )


list(APPEND platform_src ${src})
set(platform_src "${platform_src}" PARENT_SCOPE)
