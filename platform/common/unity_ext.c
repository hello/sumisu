#include "unity.h"
#include "io.h"

int UNITY_OUTPUT_CHAR(int c){
    os_printf("%c", (char)c);
    return 0;
}

int UNITY_PRINT_EOL(void){
    os_printf("\r\n");
    return 0;
}
int UNITY_OUTPUT_START(void){
    os_printf("Tests Started\r\n");
    return 0;
}
int UNITY_OUTPUT_COMPLETE(void){
    os_printf("Tests Completed\r\n");
    return 0;
}

