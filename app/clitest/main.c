#include "cmsis_os.h"
#include "io.h"
#include "sysinfo.h"
#include "cli.h"
#include "util.h"

static int _command_echo(int argc, char * argv[]){
    if(argc > 1){
        LOGI("%s\r\n", argv[1]);
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

int main(int argc, char * argv[]){
    osKernelInitialize();
    os_cli_daemon_start(PS_UART0_RX, 256, cli_command_tbl);
    osKernelStart();
    while(1){
    }
    return 0;
}
