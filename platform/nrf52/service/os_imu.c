#include "os_imu.h"
#include "cmsis_os.h"
#include "os_imu_driver.h"

static os_imu_config_t config;
static ps_topic_t topic;

static void _cli_daemon(const void * arg){
    while(1){
        os_imu_data_t data = (os_imu_data_t){0};
        data.config = &config;
        //do sampling
        ps_publish(topic, &data, sizeof(data));
        osDelay(config.sampling_period);
    }
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
    return osThreadCreate(&t, NULL);
}
