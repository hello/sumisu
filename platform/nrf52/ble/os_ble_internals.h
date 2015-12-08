#ifndef OS_BLE_INTERNALS_H
#define OS_BLE_INTERNALS_H

#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "softdevice_handler.h"

typedef void (*os_ble_init_cb)(void);
typedef void (*os_ble_event_cb)(ble_evt_t * p_ble_evt);
struct os_ble_service_t{
    os_ble_init_cb init;
    os_ble_event_cb event;
};

#endif
