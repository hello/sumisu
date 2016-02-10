#include "cmsis_os.h"
#include "io.h"
#include "sysinfo.h"
#include "os_cli.h"
#include "util.h"
#include "os_ble.h"

#define PS_NULL 0
#define PS_UART0_TX 1
#define PS_UART0_RX 2
#define PS_BLE_CONTROL 3
#define PS_BLE_EVENTS 4

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
static int _command_ble( int argc, char * argv[] ){
    int dummy = 0;
    ps_publish(PS_BLE_CONTROL, &dummy, sizeof(dummy));
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
    {"ble", _command_ble},
    {0,0},
};

static void _ble_watcher(const void * arg){
    ps_channel_t * ch = ps_subscribe(PS_BLE_EVENTS);
    while(1){
        ps_message_t * msg = NULL;
        osStatus rc = osOK;
        uint32_t err_code = 0;
        msg = ps_recv(ch, osWaitForever, &rc);
        if ( msg ){
            uint32_t opcode = 1;
            LOGD("watcher received message %s\r\n", msg->data);
            ps_free_message(msg);
        }else{
            LOGE("watcher receive error %x\r\n", rc);
        }
    }
    END_THREAD();
}
static os_ble_service_t * my_services[4];

int main(int argc, char * argv[]){
    osKernelInitialize();

    os_io_set_broadcast_topic(PS_UART0_RX);

    my_services[0] = os_ble_uart_service(PS_UART0_TX, PS_UART0_RX);
    my_services[1] = os_ble_battery_service(PS_NULL);
    my_services[2] = os_ble_device_info_service();
    os_ble_daemon_start(PS_BLE_CONTROL, PS_BLE_EVENTS,(const os_ble_service_t **)my_services);

    os_cli_daemon_start(PS_UART0_RX, 256, cli_command_tbl);

    osThreadDef_t t = (osThreadDef_t){
        .name = "watcher",
        .pthread = _ble_watcher,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 256,
    };
    osThreadCreate(&t, NULL);
    osKernelStart();
    while(1){
    }
    return 0;
}
