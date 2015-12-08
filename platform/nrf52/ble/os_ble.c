#include "os_ble.h"
#include "io.h"
#include "heap.h"
#include "util.h"
#include <stdbool.h>
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "ble_config.h"
#include "os_ble_internals.h"

struct{
    ps_topic_t tout;
    ps_topic_t tin;
    volatile bool ready;
    const os_ble_service_t ** services;
}self;


/**
 * ADVERTISING CB
 */
static void _on_adv_evt(ble_adv_evt_t ble_adv_evt){
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            ps_publish(self.tout, "advertising", sizeof("advertising"));
            break;
        case BLE_ADV_EVT_IDLE:
            ps_publish(self.tout, "idle", sizeof("idle"));
            break;
        default:
            break;
    }
}
/**
 * CONNECTION CB
 */
static void _on_ble_evt(ble_evt_t * p_ble_evt){
    uint32_t                         err_code;
    switch (p_ble_evt->header.evt_id){
        case BLE_GAP_EVT_CONNECTED:
            /*
             *m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
             */
            ps_publish(self.tout, "connected", sizeof("connected"));
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            /*
             *m_conn_handle = BLE_CONN_HANDLE_INVALID;
             */
            ps_publish(self.tout, "disconnected", sizeof("disconnected"));
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            /*
             *err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
             *APP_ERROR_CHECK(err_code);
             */
            ps_publish(self.tout, "securereq", sizeof("seqreq"));
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            /*
             *err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
             *APP_ERROR_CHECK(err_code);
             */
            break;

        default:
            // No implementation needed.
            break;
    }
}
/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void _on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    /*
     *if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
     *{
     *    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
     *    APP_ERROR_CHECK(err_code);
     *}
     */
}
static void _conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}
/**
 * MAIN ROUTING FOR ALL CBS
 */
static void _ble_evt_dispatch(ble_evt_t * p_ble_evt){
    //stack provided connection param change
    ble_conn_params_on_ble_evt(p_ble_evt);
    //keeps track locally of connected devices
    _on_ble_evt(p_ble_evt);
    //advertising, will dispatch events via on_adv_evt callback
    ble_advertising_on_ble_evt(p_ble_evt);

    if( self.services ){
        const os_ble_service_t ** itr = self.services;
        while(*itr){
            if((*itr)->event){
                (*itr)->event(p_ble_evt);
            }
            itr++;
        }
    }
}

/**
 * BLE_CONTROLLER DAEMON
 */
static void _ble_daemon(const void * arg){
    ps_channel_t * ch = ps_subscribe(self.tin);
    while(ch){
        ps_message_t * msg = NULL;
        osStatus rc = osOK;
        uint32_t err_code = 0;
        msg = ps_recv(ch, osWaitForever, &rc);
        if ( msg ){
            uint32_t opcode = 1;
            LOGD("BLEd received message %u\r\n", msg->sz);
            switch( opcode ){
                case 1:
                    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
                    APP_ERROR_CHECK(err_code);
                    break;
            }
            ps_free_message(msg);
        }else{
            LOGE("BLEd receive error %x\r\n", rc);
        }
    }
    LOGE("BLEd Failed\r\n");
    END_THREAD();
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void _conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = _on_conn_params_evt;
    cp_init.error_handler                  = _conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}
/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of 
 *          the device. It also sets the permissions and appearance.
 */
static void _gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}
/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void _advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    /*
     *scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
     *scanrsp.uuids_complete.p_uuids  = m_adv_uuids;
     */

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, &scanrsp, &options, _on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void _ble_sd_init( void ){
    uint32_t                err_code;
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#if (defined(S130) || defined(S132))
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(_ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/*
 * dispatch to initialize all ble stack
 */
static void _init_ble( const os_ble_service_t * services[]){
    _ble_sd_init();
    _gap_params_init();
    _advertising_init();
    _conn_params_init();
    if( services ){
        const os_ble_service_t ** itr = services;
        while(*itr){
            if((*itr)->init){
                (*itr)->init();
            }
            itr++;
        }
    }
}

osStatus os_ble_daemon_start(ps_topic_t status, ps_topic_t control, const os_ble_service_t ** services){
    osThreadDef_t t = (osThreadDef_t){
        .name = "bled",
        .pthread = _ble_daemon,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 256,
    };
    if( !self.ready ){
        _init_ble(services);
    }
    self.tin = control;
    self.tout = status;
    self.ready = true;
    self.services = services;
    if( osThreadCreate(&t, NULL) ){
        return osOK;
    }else{
        return osErrorOS;
    }
}
