#include "os_ble.h"
#include "os_ble_internals.h"
#include "pubsub.h"
#include "io.h"

static void _init(void);
static void _event(ble_evt_t * p_ble_evt);

static struct{
    os_ble_service_t service;
    ps_topic_t rx;
    ps_topic_t tx;
}self;

static void _init(void){
    LOGD("Init uart service\r\n");
}

static void _event(ble_evt_t * p_ble_evt){
    LOGD("UART event\r\n");
}

os_ble_service_t * os_ble_uart_service(ps_topic_t rx, ps_topic_t tx){
    self.service = (os_ble_service_t){
        .init = _init,
        .event = _event,
    };
    self.rx = rx;
    self.tx = tx;
    return &self.service;
}

