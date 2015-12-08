#ifndef PUBSUB_CHANNELS_H
#define PUBSUB_CHANNELS_H

typedef enum{
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
