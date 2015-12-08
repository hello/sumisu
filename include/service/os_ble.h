#ifndef HLO_BLE_H
#define HLO_BLE_H
#include "pubsub.h"

/**
 * topic: the topic in which ble status are sent
 * @status: the topic in which the daemon will broadcast the status to the app
 * @control: the topic in which the daemon will listen from the app
 */

osStatus os_ble_daemon_start(ps_topic_t status, ps_topic_t control);

#endif
