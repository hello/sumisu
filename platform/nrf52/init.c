#include "softdevice_handler.h"
#include "cmsis_os.h"
#include "io.h"
#include "nrf_delay.h"

osStatus osKernelInitialize (void){
    //setup clksrc for kernel ticks
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, NULL);

    //debounce power
    nrf_delay_ms(500);

    //setup uart
    os_uart_init();
    LOGI("Welcome to sumisu - nrf52\r\n");
}
