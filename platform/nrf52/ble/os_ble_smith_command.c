#include "os_ble.h"
#include "os_ble_internals.h"
#include "cmsis_os.h"
#include "util.h"
#include <string.h>
//                                                                                                                   xx    xx     1     0
#define BLE_UUID_SMITH_COMMAND_SERVICE_BASE {0x70, 0x26, 0xC5, 0x91, 0x2A, 0x4E, 0x4A, 0x68, 0xA4, 0x34, 0xF3, 0xD9, 0xF3, 0xD7, 0xAB, 0xAA}
#define BLE_UUID_SMITH_COMMAND_SERVICE      0x0001
#define BLE_UUID_SMITH_COMMAND_READ_CHAR    0x0002
#define BLE_UUID_SMITH_COMMAND_WRITE_CHAR   0x0003

static struct{
    os_ble_service_t service;
    uint16_t service_handle;
    uint16_t conn_handle;
    ps_topic_t listen, publish;
    ble_gatts_char_handles_t smith_read_char_handle;
    ble_gatts_char_handles_t smith_write_char_handle;
    uint8_t client_buf[BLE_GATTS_VAR_ATTR_LEN_MAX];
}self;

static uint32_t _init(void);
static uint32_t _event(ble_evt_t * p_ble_evt);

//adds tx charactersitic to smith(phone writes to smith)
static uint32_t _add_smith_write_characteristic(void){
    ble_uuid_t          ble_uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;

    // The ble_gatts_char_md_t structure uses bit fields. So we reset the memory to zero.
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write  = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_SMITH_COMMAND_WRITE_CHAR);

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 0;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_GATTS_VAR_ATTR_LEN_MAX;
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(self.service_handle, &char_md, &attr_char_value, &self.smith_write_char_handle);

}
//adds rx characteristic to smith (phone reads from smith)
static uint32_t _add_smith_read_characteristic(void){
    ble_uuid_t          ble_uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;

    // The ble_gatts_char_md_t structure uses bit fields. So we reset the memory to zero.
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read  = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_SMITH_COMMAND_READ_CHAR);

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 0;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_GATTS_VAR_ATTR_LEN_MAX;
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(self.service_handle, &char_md, &attr_char_value, &self.smith_read_char_handle);
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
    err = _add_smith_read_characteristic();
    if (err != NRF_SUCCESS){
        return err;
    }
    err = _add_smith_write_characteristic();
    if (err != NRF_SUCCESS){
        return err;
    }
    return NRF_SUCCESS;
}
static void _on_write(const ble_gatts_evt_write_t * evt){
    if(evt->op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW ){
        typedef struct __attribute__((packed)){
            uint16_t att_handle;
            uint16_t offset;
            uint16_t length;
            uint8_t value[0];
        }queued_write_block_t;
        queued_write_block_t * block = (queued_write_block_t*)self.client_buf;
        /*
         *LOGD("Long Write handle %u: %d bytes, offset %d\r\n", block->att_handle, evt->len, block->offset);
         */
        PRINT_HEX(self.client_buf, 40);
        PRINT_HEX(evt, sizeof(*evt));
    }
    if(evt->handle == self.smith_write_char_handle.value_handle){
        LOGD("Write OP %u: %d bytes, offset %d\r\n", evt->op, evt->len, evt->offset);
    }
}
uint32_t _event(ble_evt_t * p_ble_evt){
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            self.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            self.conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
        case BLE_GATTS_EVT_WRITE:
            {
                LOGD("EVT Write\r\n");
                ble_gatts_evt_write_t write_evt = p_ble_evt->evt.gatts_evt.params.write;
                _on_write(&write_evt);
            }
            break;
        case BLE_EVT_USER_MEM_REQUEST:
            {
                //TODO: WARNING this is not safe if multiple services enable long writes
                ble_user_mem_block_t block = (ble_user_mem_block_t){
                    .p_mem = self.client_buf,
                    .len = sizeof(self.client_buf),
                };
                memset(self.client_buf, 0, sizeof(self.client_buf));
                uint32_t ret = sd_ble_user_mem_reply(self.conn_handle, &block);
                LOGD("Req %u\r\n", ret);
            }
            break;
        case BLE_EVT_USER_MEM_RELEASE:
            LOGD("Mem Release\r\n");
            break;
        default:
            // No implementation needed.
            break;
    }
}

static void _smith_command_service_task(const void * arg){
    ps_channel_t * ch = ps_subscribe(self.listen);
    ps_message_t * msg = NULL;
    ps_message_t * last_msg = NULL;
    uint32_t ret;
    while( (msg = ps_recv(ch, osWaitForever, NULL)) ){
        //update the value
        ble_gatts_value_t v = {
            .len = msg->sz,
            .offset = 0,
            .p_value = msg->data,
        };
        sd_ble_gatts_value_set(self.conn_handle, self.smith_read_char_handle.value_handle, &v);

        if(last_msg){//message remains valid until new one comes in
            LOGD("Freeing old message\r\n");
            ps_free_message(last_msg);
        }
        last_msg = msg;

        //lastly notify the connected client of the new value
        uint16_t len = msg->sz;
        ble_gatts_hvx_params_t notify = (ble_gatts_hvx_params_t){
            .handle = self.smith_read_char_handle.value_handle,
            .type = BLE_GATT_HVX_NOTIFICATION,
            .offset = 0,
            .p_len = &len,
            .p_data = msg->data,
        };
        ret = sd_ble_gatts_hvx(self.conn_handle, &notify);
    }
    LOGE("Smith daemon unexpectedly exited\r\n");
    END_THREAD();
}

os_ble_service_t * os_ble_smith_command_service(ps_topic_t from_client, ps_topic_t to_client){
    self.service = (os_ble_service_t){
        .init = _init,
        .event = _event,
    };
    self.listen = to_client;
    self.publish = from_client;
    START_THREAD("ble_protod", _smith_command_service_task, 256, NULL);
    return &self.service;
}
