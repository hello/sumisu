#pragma once
#include "pubsub.h"

typedef struct{
    //config stuff here
    uint32_t sampling_period;
}os_imu_config_t;

typedef struct{
    const os_imu_config_t * config;
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t rx;
    int32_t ry;
    int32_t rz;
}os_imu_data_t;

osStatus os_imu_daemon_start(ps_topic_t broadcast, const os_imu_config_t * config);

