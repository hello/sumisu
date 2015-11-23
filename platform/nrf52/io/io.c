#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_drv_uart.h"

static int uart_ready;

void uart_event_handler(nrf_drv_uart_event_t * p_event, void * p_context){
}

void os_uart_init(void){
    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    /*
     *ret_code_t ret = nrf_drv_uart_init(&config, uart_event_handler);
     */
    ret_code_t ret = nrf_drv_uart_init(&config, NULL);
}
void os_printf(const char * format, ...){
    if(!uart_ready){
        os_uart_init();
        uart_ready = 1;
    }
    nrf_drv_uart_tx("Hello", 4);
}
