#include "cmsis_os.h"
#include "io.h"
#include "sysinfo.h"
#include "os_cli.h"
#include "util.h"

#define PS_UART0_RX 0

int main(int argc, char * argv[]){
    osKernelInitialize();

    os_io_set_broadcast_topic(PS_UART0_RX);

    os_cli_daemon_start(PS_UART0_RX, 256, NULL);

    osKernelStart();
    while(1){
    }
    return 0;
}
