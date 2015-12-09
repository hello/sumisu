#include "os_ble.h"
#include "os_ble_internals.h"
#include "ble_bas.h"


static struct{
    os_ble_service_t service;
    ble_bas_t bas;
}self;

static uint32_t _init(void);
static uint32_t _event(ble_evt_t * p_ble_evt);
static void _bas_event(ble_bas_t * p_bas, ble_bas_evt_t * p_evt);

static void _bas_event(ble_bas_t * p_bas, ble_bas_evt_t * p_evt){

}

static uint32_t _init(void){
    ble_bas_init_t init = (ble_bas_init_t){
        .evt_handler = _bas_event,                    /**< Event handler to be called for handling events in the Battery Service. */
        .support_notification = false,           /**< TRUE if notification of Battery Level measurement is supported. */
        .p_report_ref = NULL,                   /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Battery Level characteristic */
        .initial_batt_level = 100,             /**< Initial battery level */
    };
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&init.battery_level_report_read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&init.battery_level_char_attr_md.write_perm);
    return ble_bas_init(&self.bas, &init);
}

static uint32_t _event(ble_evt_t * p_ble_evt){
    ble_bas_on_ble_evt(&self.bas, p_ble_evt);
    return 0;
}

os_ble_service_t * os_ble_battery_service(ps_topic_t listen){
    self.service = (os_ble_service_t){
        .init = _init,
        .event = _event,
    };
    return &self.service;
}
