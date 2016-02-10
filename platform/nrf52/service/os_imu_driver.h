#include <stdbool.h>
/**
 * driver abstraction for differnet IMUs
 */
#include "os_imu.h"
/**
 * driver init, only called once per reset
 */
osStatus os_imu_driver_init(const os_imu_config_t * config);

/**
 * resets the IMU to default state, may be called multiple times per reset
 */
osStatus os_imu_driver_reset(void);

/**
 * reads the out_data
 */
osStatus os_imu_driver_read(os_imu_data_t * out_data);

/**
 * returns True if data is ready, may block
 * not implemented yet
 */
bool os_imu_driver_data_ready(void);


