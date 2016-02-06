#include "cmsis_os.h"
#include "io.h"
#include "sysinfo.h"
#include "os_cli.h"
#include "os_imu.h"
#include "util.h"

#define IMU_TOPIC 0
#define CLI_TOPIC 1

static void _imu_watcher(void const * arg){
    ps_channel_t * ch = ps_subscribe(IMU_TOPIC);
    ps_message_t * msg;
    while( (msg = ps_recv(ch, osWaitForever, NULL))){
        os_imu_data_t * d = (os_imu_data_t*)msg->data;
        LOGI("IMU, X %u, Y %u, Z %u\r\n", d->x, d->y, d->z);
        ps_free_message(msg);
    }
    LOGE("An error has occured :(\r\n");
    END_THREAD();
}

int main(int argc, char * argv[]){
    osKernelInitialize();

    os_io_set_broadcast_topic(CLI_TOPIC);

    os_cli_daemon_start(CLI_TOPIC, 256, NULL);

    {
        os_imu_config_t config = (os_imu_config_t){
            .sampling_period = 100,
        };
        os_imu_daemon_start(IMU_TOPIC, &config);
        START_THREAD("watcher", _imu_watcher, 256, NULL);
    }

    osKernelStart();
    while(1){
    }
    return 0;
}
