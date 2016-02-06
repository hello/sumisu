#include "os_imu.h"


osStatus os_imu_driver_init(const os_imu_config_t * config);

osStatus os_imu_driver_reset(void);

osStatus os_imu_driver_read(os_imu_data_t * out_data);


