/*
 *#include <stdio.h>
 */
#include "test.h"
#include "dtm.h"
#include "io.h"

int main(int argc, char * argv[]){
    os_uart_init();
    os_printf("Welcome to sumisu\r\n");
    osDelay(1000);
    test_rtos();
    for(;;);
    return 0;
}
