/*
 *#include <stdio.h>
 */
#include "test.h"
#include "dtm.h"
#include "io.h"
#include "cmsis_os.h"

static void test1(void const * arg){
    while(1){
        os_printf("T1\r\n");
        osDelay(1000);
        os_printf("ET1\r\n");
    }
}
static void test2(void const * arg){
    while(1){
        os_printf("R1\r\n");
        osDelay(1000);
    }
}
int main(int argc, char * argv[]){
    osThreadDef_t t1 = (osThreadDef_t){
        .name = "test1",
        .pthread = test1,
        .tpriority = 0,
        .instances = 1,
        .stacksize = 512,
    };

    osKernelInitialize();
    osThreadCreate(&t1, 0);
    osKernelStart();
    for(;;);
    return 0;
}
