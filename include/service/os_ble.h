#ifndef HLO_BLE_H
#define HLO_BLE_H
#include "pubsub.h"

typedef struct os_ble_service_t os_ble_service_t;
/**
 * topic: the topic in which ble status are sent
 * @status: the topic in which the daemon will broadcast the status to the app
 * @control: the topic in which the daemon will listen from the app
 */
osStatus os_ble_daemon_start(ps_topic_t listen, ps_topic_t broadcast, const os_ble_service_t ** services);

/**
 * UART service
 */
os_ble_service_t * os_ble_uart_service(ps_topic_t listen, ps_topic_t publish);
/**
 * Battery service
 * @listen: channel which messages come in
 */
os_ble_service_t * os_ble_battery_service(ps_topic_t listen);
/**
 * Device information service
 * device information is same for all sumisu builds
 */
os_ble_service_t * os_ble_device_info_service(void);
/**
 * Smith command service
 * @from_client: will broadcast decoded from client(phone) to smith
 * @to_client:   will encode protobuf and send to client(phone)
 */
os_ble_service_t * os_ble_smith_command_service(ps_topic_t from_client, ps_topic_t to_client);



#endif
