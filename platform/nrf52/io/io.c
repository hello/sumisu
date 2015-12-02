#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_drv_uart.h"
#include "util.h"
#include "app_fifo.h"
#include "nrf_drv_config.h"

static const char * const g_pcHex = "0123456789ABCDEF";

static uint8_t echo_buffer[1];
static uint8_t output_buffer[1];
static app_fifo_t out;

static void _uart_event_handler(nrf_drv_uart_event_t * p_event, void * p_context);

void os_putc(char c){
    app_fifo_put(&out, c);
    nrf_drv_uart_tx("", 1);
}

static void _uart_event_handler(nrf_drv_uart_event_t * p_event, void * p_context){
    switch(p_event->type){
        case NRF_DRV_UART_EVT_TX_DONE:
            if(NRF_SUCCESS == app_fifo_get(&out, output_buffer)){
                nrf_drv_uart_tx(output_buffer, 1);
            }
            break;
        case NRF_DRV_UART_EVT_RX_DONE:
            if(is_ascii(*echo_buffer)){
                nrf_drv_uart_tx(echo_buffer, 1);
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
    ret_code_t ret = nrf_drv_uart_init(&config, _uart_event_handler);
    nrf_drv_uart_rx(echo_buffer, 1);
    {
        static char out_buf[UART0_OUTBUF_SIZE];
        app_fifo_init(&out, out_buf, sizeof(out_buf));
    }
}
