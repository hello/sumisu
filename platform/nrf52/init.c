#include "softdevice_handler.h"
#include "cmsis_os.h"
#include "io.h"
#include "nrf_delay.h"
#include "crypto.h"
#include "pubsub.h"

osStatus osKernelInitialize (void){
    osStatus ret = osOK;

    //setup clksrc for kernel ticks
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, NULL);

    //debounce power
    nrf_delay_ms(500);

    //initialize pubsub service
    ret = ps_init();

    //seed some entropy
    //TODO get entropy from nrf_rand lib
    os_rand_add_entroy(1);

    //setup uart
    os_uart_init();
    LOGI("Welcome to sumisu - nrf52\r\n");

    return ret;
}
