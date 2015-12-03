#include "sysinfo.h"
#include "FreeRTOS.h"

uint64_t uptime(void){
    return (uint64_t)(xTaskGetTickCount() / portTICK_RATE_MS);
}
