#include "os_ble.h"
#include "os_ble_internals.h"
#include "cmsis_os.h"
#include <string.h>

#define BLE_UUID_SMITH_COMMAND_SERVICE_BASE {0x79, 0x26, 0xC5, 0x91, 0x2A, 0x4E, 0x4A, 0x68, 0xA4, 0x34, 0xF3, 0xD9, 0xF3, 0xD7, 0xAB, 0x79}
#define BLE_UUID_SMITH_COMMAND_SERVICE      0x0001
#define BLE_UUID_SMITH_COMMAND_WRITE_CHAR   0x0002

static struct{
    os_ble_service_t service;
    uint16_t service_handle;
}self;

static uint32_t _init(void);
static uint32_t _event(ble_evt_t * p_ble_evt);

static uint32_t _add_characteristic(uint16_t uuid){
/*
 *    ble_uuid_t          ble_uuid;
 *    ble_gatts_char_md_t char_md;
 *    ble_gatts_attr_t    attr_char_value;
 *    ble_gatts_attr_md_t attr_md;
 *
 *    // The ble_gatts_char_md_t structure uses bit fields. So we reset the memory to zero.
 *    memset(&char_md, 0, sizeof(char_md));
 *
 *    char_md.char_props.read  = 1;
 *    char_md.p_char_user_desc = NULL;
 *    char_md.p_char_pf        = NULL;
 *    char_md.p_user_desc_md   = NULL;
 *    char_md.p_cccd_md        = NULL;
 *    char_md.p_sccd_md        = NULL;
 *
 *    BLE_UUID_BLE_ASSIGN(ble_uuid, uuid);
 *
 *    memset(&attr_md, 0, sizeof(attr_md));
 *
 *    attr_md.read_perm  = dis_attr_md->read_perm;
 *    attr_md.write_perm = dis_attr_md->write_perm;
 *    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
 *    attr_md.rd_auth    = 0;
 *    attr_md.wr_auth    = 0;
 *    attr_md.vlen       = 0;
 *
 *    memset(&attr_char_value, 0, sizeof(attr_char_value));
 *
 *    attr_char_value.p_uuid    = &ble_uuid;
 *    attr_char_value.p_attr_md = &attr_md;
 *    attr_char_value.init_len  = char_len;
 *    attr_char_value.init_offs = 0;
 *    attr_char_value.max_len   = char_len;
 *    attr_char_value.p_value   = p_char_value;
 *
 *    return sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value, p_handles);
 */
}
uint32_t _init(void){
    uint32_t err;
    uint8_t vtype;
    LOGD("Init Smith Command Service\r\n");
    ble_uuid128_t smith_base = BLE_UUID_SMITH_COMMAND_SERVICE_BASE;
    err = sd_ble_uuid_vs_add(&smith_base, &vtype);
    if( err != NRF_SUCCESS ){
        return err;
    }
    ble_uuid_t ble_uuid = (ble_uuid_t){
        .uuid = BLE_UUID_SMITH_COMMAND_SERVICE,
        .type = vtype,
    };
    err = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &self.service_handle);
    if (err != NRF_SUCCESS){
        return err;
    }
    /*
     *err = _add_charactertistic(BLE_UUID_SMITH_COMMAND_WRITE_CHAR,
     *if (err != NRF_SUCCESS){
     *    return err;
     *}
     */
    return NRF_SUCCESS;
}
uint32_t _event(ble_evt_t * p_ble_evt){

}

os_ble_service_t * os_ble_smith_command_service(ps_topic_t from_client, ps_topic_t to_client){
    self.service = (os_ble_service_t){
        .init = _init,
        .event = _event,
    };
    return &self.service;
}
