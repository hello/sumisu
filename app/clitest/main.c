#include "cmsis_os.h"
#include "io.h"
#include "sysinfo.h"
#include "cli.h"
#include "util.h"


int main(int argc, char * argv[]){
    osKernelInitialize();
    os_cli_daemon_start(PS_UART0_RX, 1, 256);
    osKernelStart();
    while(1){
    }
    return 0;
}
