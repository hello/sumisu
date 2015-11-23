#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_drv_uart.h"
#include "util.h"

static uint8_t echo_buffer[1];
static uint8_t output_buffer[1];

void uart_event_handler(nrf_drv_uart_event_t * p_event, void * p_context){
    switch(p_event->type){
        case NRF_DRV_UART_EVT_TX_DONE:
            if(!isBufferEmpty(&out)){
                nrf_drv_uart_tx("", 0);
            }
            break;
        case NRF_DRV_UART_EVT_RX_DONE:
            if(is_ascii(*echo_buffer)){
                nrf_drv_uart_tx("", 1);
            }
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
    char * itr = format;
    while(*itr){
        itr++;
    }
}
