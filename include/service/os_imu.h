#pragma once
#include "pubsub.h"

typedef struct{
    //config stuff here
    uint32_t sampling_period;
}os_imu_config_t;

typedef struct{
    const os_imu_config_t * config const;
    uint32_t x;
    uint32_t y;
    uint32_t z;
}os_imu_data_t;

osStatus os_imu_daemon_start(ps_topic_t broadcast, const os_imu_config_t * config);

