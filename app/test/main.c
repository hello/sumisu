/*
 *#include <stdio.h>
 */
#include "test.h"
#include "dtm.h"
#include "io.h"

int main(int argc, char * argv[]){
    osDelay(500);
    os_uart_init();
    os_printf("GGGGGGGGGGGGGGGGAAAAAAAAAAA\r\n");
    osDelay(500);
    os_printf("%s %d %x\r\n", "Hello again", 123, 0xFF);
    while(1){}
    return 0;
}
