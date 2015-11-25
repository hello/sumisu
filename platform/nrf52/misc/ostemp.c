#include "cmsis_os.h"
#include "nrf_delay.h"
/**
 * temporary hack to implement some cmsis os functions
 */

osStatus osDelay (uint32_t millisec){
    nrf_delay_ms(millisec);
    return osOK;
}
