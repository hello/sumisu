#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_drv_uart.h"

static int uart_ready;
static uint8_t echo_buffer[1];

void uart_event_handler(nrf_drv_uart_event_t * p_event, void * p_context){
    switch(p_event->type){
        case NRF_DRV_UART_EVT_TX_DONE:
            break;
        case NRF_DRV_UART_EVT_RX_DONE:
            nrf_drv_uart_tx(echo_buffer, 1);
            nrf_drv_uart_rx(echo_buffer, 1);
            break;
        default:
        case NRF_DRV_UART_EVT_ERROR:
            break;
    }
}

void os_uart_init(void){
    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    ret_code_t ret = nrf_drv_uart_init(&config, uart_event_handler);
    nrf_drv_uart_rx(echo_buffer, 1);
}
void os_printf(const char * format, ...){
    if(!uart_ready){
        os_uart_init();
        uart_ready = 1;
    }
    nrf_drv_uart_tx("Hello", 4);
}
