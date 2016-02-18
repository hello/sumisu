#include "os_imu.h"
#include "cmsis_os.h"
#include "os_imu_driver.h"
#include "util.h"

static os_imu_config_t config;
static ps_topic_t topic;

static void _imu_daemon(const void * arg){
    ASSERT_OK(os_imu_driver_init(&config));
    while(1){
        os_imu_data_t data = (os_imu_data_t){0};
        data.config = &config;

        os_imu_driver_read(&data);

        ps_publish(topic, &data, sizeof(data));

        osDelay(config.sampling_period);
    }
    END_THREAD();
}
osStatus os_imu_daemon_start(ps_topic_t broadcast, const os_imu_config_t * user_config){
    config = *user_config; 
    osThreadDef_t t = (osThreadDef_t){
        .name = "imud",
        .pthread = _imu_daemon,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 128,
    };
    osThreadCreate(&t, NULL);
    return osOK;
}
