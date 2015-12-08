#ifndef PUBSUB_TOPICS_H
#define PUBSUB_TOPICS_H

typedef enum{
    PS_NULL = 0,            //0 is NULL as a service
    PS_SYSTEM,              //indicates system events
    PS_TEST_0,              //for testing
    PS_UART0_RX,            //uart input
    PS_BLE_EVENTS,          //BLE events
    PS_BLE_CONTROL,         //Control BLE events
    PS_BLE_CLI_RX,          //BLE CLI Service input
    PS_BLE_CLI_TX,          //BLE CLI service output
    PS_TOPIC_SIZE,
}ps_topic_t;

#endif
