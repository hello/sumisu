#include "os_imu_driver.h"
#include "crypto.h"

osStatus os_imu_driver_init(const os_imu_config_t * config){
    return osOK;
}

osStatus os_imu_driver_reset(void){
    return osOK;
}

osStatus os_imu_driver_read(os_imu_data_t * out_data){
    out_data->x = os_rand() % 64;
    out_data->y = os_rand() % 64;
    out_data->z = os_rand() % 64;
    return osOK;
}
