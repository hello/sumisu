#include "os_ble.h"
#include "os_ble_internals.h"
#include "cmsis_os.h"
#include "ble_dis.h"
#include "sysinfo.h"


static struct{
    os_ble_service_t service;
}self;

static uint32_t _init(void);
static uint32_t _event(ble_evt_t * p_ble_evt);

uint32_t _init(void){
    ble_dis_init_t dis_init;
    memset(&dis_init, sizeof(dis_init), 0);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);
    return ble_dis_init(&dis_init);
}
uint32_t _event(ble_evt_t * p_ble_evt){
return 0;
}

os_ble_service_t * os_ble_device_info_service(void){
    self.service = (os_ble_service_t){
        .init = _init,
        .event = _event,
    };
    return &self.service;
}
