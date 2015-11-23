/*
 *#include <stdio.h>
 */
#include "test.h"
#include "dtm.h"
#include "io.h"

int main(int argc, char * argv[]){
    osDelay(500);
    os_uart_init();
    os_printf("hello");
    while(1){}
    return 0;
}
