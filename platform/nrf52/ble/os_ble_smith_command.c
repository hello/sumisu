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
}self;

static uint8_t write_buf[BLE_GATTS_VAR_ATTR_LEN_MAX];
static uint16_t write_sz;

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
    uint8_t vtype;
    LOGD("Init Smith Command Service\r\n");
    ble_uuid128_t smith_base = BLE_UUID_SMITH_COMMAND_SERVICE_BASE;
    APP_ERROR_CHECK(sd_ble_uuid_vs_add(&smith_base, &vtype));

    ble_uuid_t ble_uuid = (ble_uuid_t){
        .uuid = BLE_UUID_SMITH_COMMAND_SERVICE,
        .type = vtype,
    };
    APP_ERROR_CHECK(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &self.service_handle));

    APP_ERROR_CHECK(_add_smith_read_characteristic());
    APP_ERROR_CHECK(_add_smith_write_characteristic());

    return NRF_SUCCESS;
}
static uint16_t _copy_buffer(uint8_t * buf, uint16_t * sz, uint16_t max_size, uint16_t in_len, uint16_t in_offset, uint8_t * in_data){
    if(*sz != in_offset){
        LOGE("Offset mismatch\r\n");
        return BLE_GATT_STATUS_ATTERR_INVALID_OFFSET;
    }else if( *sz + in_len >= max_size ){
        LOGE("Buffer limit reached\r\n");
        return BLE_GATT_STATUS_ATTERR_INVALID_OFFSET;
    }else{
        memcpy(buf + in_offset, in_data, in_len);
        *sz += in_len;
        return BLE_GATT_STATUS_SUCCESS;
    }
}
static void _clear_write_buffer(void){
    memset(write_buf, 0, sizeof(write_buf));
    write_sz = 0;
}
//for long writes
static void _on_request(const ble_gatts_evt_rw_authorize_request_t * req){
    if(req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE){
        ble_gatts_evt_write_t * wr = &(req->request.write);

        ble_gatts_rw_authorize_reply_params_t auth;
        memset(&auth, 0, sizeof(auth));
        auth.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;

        switch(wr->op){
            case BLE_GATTS_OP_PREP_WRITE_REQ:
                if( wr->handle == self.smith_write_char_handle.value_handle ){
                    auth.params.write.gatt_status = _copy_buffer(write_buf, &write_sz, sizeof(write_buf), wr->len, wr->offset, wr->data);
                    APP_ERROR_CHECK(sd_ble_gatts_rw_authorize_reply(self.conn_handle, &auth));
                }
                break;
            case BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL:
                _clear_write_buffer();
                break;
            case BLE_GATTS_OP_EXEC_WRITE_REQ_NOW:
                if( write_sz ){
                    auth.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
                    APP_ERROR_CHECK(sd_ble_gatts_rw_authorize_reply(self.conn_handle, &auth));
                    ps_publish(self.publish, write_buf, write_sz);
                }
                _clear_write_buffer();
                break;
            default:
                break;
        }
    }
}
//for short writes
static void _on_write(const ble_gatts_evt_write_t * evt){
    if(evt->handle == self.smith_write_char_handle.value_handle){
        ps_publish(self.publish, evt->data, evt->len);
    }
}
uint32_t _event(ble_evt_t * p_ble_evt){
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            self.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            _clear_write_buffer();
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            self.conn_handle = BLE_CONN_HANDLE_INVALID;
            _clear_write_buffer();
            break;
        case BLE_GATTS_EVT_WRITE:
            {
                ble_gatts_evt_write_t * write_evt = &(p_ble_evt->evt.gatts_evt.params.write);
                _on_write(write_evt);
            }
            break;
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            {
                ble_gatts_evt_rw_authorize_request_t * req = &(p_ble_evt->evt.gatts_evt.params.authorize_request);
                _on_request(req);
            }
            break;
        case BLE_EVT_USER_MEM_REQUEST:
            {
                //TODO: WARNING this is not safe if multiple services enable long writes
                APP_ERROR_CHECK(sd_ble_user_mem_reply(self.conn_handle, NULL));
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
        sd_ble_gatts_hvx(self.conn_handle, &notify);
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
