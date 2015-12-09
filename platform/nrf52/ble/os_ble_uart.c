#include "os_ble.h"
#include "cmsis_os.h"
#include "os_ble_internals.h"
#include "pubsub.h"
#include "io.h"
#include "ble_nus.h"
#include "io.h"
#include "util.h"

#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */
static uint32_t _init(void);
static uint32_t _event(ble_evt_t * p_ble_evt);

static struct{
    os_ble_service_t service;
    ps_topic_t listen;
    ps_topic_t publish;
    ble_nus_t  m_nus;                                      /**< Structure to identify the Nordic UART Service. */
}self;


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
static void _nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length){
    ps_publish(self.publish, p_data, length);
}
static void _watcher(const void * arg){
    ps_channel_t * ch = ps_subscribe(self.listen);
    while(ch){
        ps_message_t * msg = NULL;
        osStatus rc = osOK;
        uint32_t err_code = 0;
        msg = ps_recv(ch, osWaitForever, &rc);
        if ( msg ){
            err_code = ble_nus_string_send(&self.m_nus, msg->data, msg->sz);
            
            ps_free_message(msg);
        }else{
            LOGE("BLE_UARTd receive error %x\r\n", rc);
        }
    }
    LOGE("BLE_UART watcher exited");
    END_THREAD();
}
static uint32_t _init(void){
    LOGD("Init uart service\r\n");
    uint32_t       err_code;
    ble_nus_init_t nus_init;
    
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = _nus_data_handler;
    
    err_code = ble_nus_init(&self.m_nus, &nus_init);

    osThreadDef_t t = (osThreadDef_t){
        .name = "BLE_UARTd",
        .pthread = _watcher,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 256,
    };
    osThreadCreate(&t, NULL);
    return err_code;
}

static uint32_t _event(ble_evt_t * p_ble_evt){
    ble_nus_on_ble_evt(&self.m_nus, p_ble_evt);
    return 0;
}

os_ble_service_t * os_ble_uart_service(ps_topic_t listen, ps_topic_t publish){
    self.service = (os_ble_service_t){
        .init = _init,
        .event = _event,
    };
    self.listen = listen;
    self.publish = publish;
    return &self.service;
}

