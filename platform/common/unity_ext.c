#include "unity.h"
#include "io.h"

int UNITY_OUTPUT_CHAR(int c){
    LOGT("%c",c);
    return 0;
}

int UNITY_PRINT_EOL(void){
    LOGT("\r\n");
    return 0;
}
int UNITY_OUTPUT_START(void){
    LOGT("Tests Started\r\n");
    return 0;
}
int UNITY_OUTPUT_COMPLETE(void){
    LOGT("Tests Completed\r\n");
    return 0;
}

