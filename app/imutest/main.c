#include "cmsis_os.h"
#include "io.h"
#include "sysinfo.h"
#include "os_cli.h"
#include "os_imu.h"
#include "util.h"

#define IMU_TOPIC 0
#define CLI_TOPIC 1

static int _command_echo(int argc, char * argv[]){
    if(argc > 1){
        int i;
        for(i = 1; i < argc; i++){
            LOGI("%s ", argv[i]);
        }
        LOGI("\r\n");
    }
    return 0;
}
#include "heap.h"
static int _command_free(int argc, char * argv[]){
    LOGI("Free %u\r\n", os_free_heap_size());
    return 0;
}
static cli_command_node_t cli_command_tbl[] = {
    {"echo", _command_echo,},
    {"free", _command_free,},
    {0,0},
};


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

    os_cli_daemon_start(CLI_TOPIC, 256, cli_command_tbl);

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
