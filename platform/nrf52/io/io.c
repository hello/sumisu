#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_drv_uart.h"
#include "util.h"
#include "app_fifo.h"
#include "nrf_drv_config.h"
#include "pubsub.h"

#define COMMAND_BUFFER_SIZE 64
static const char * const g_pcHex = "0123456789ABCDEF";

static uint8_t echo_buffer[1];
static uint8_t output_buffer[1];
static uint8_t command_buffer[COMMAND_BUFFER_SIZE+1];
static uint32_t command_buffer_idx;
static app_fifo_t out;
static ps_topic_t out_topic;

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
            switch(*echo_buffer){
                case 127://del character
                case 0x08://backspace
                    command_buffer[command_buffer_idx] = 0;
                    if( command_buffer_idx ){
                        command_buffer_idx -= 1;
                    }
                    break;
                case '\r':
                case '\n':
                    nrf_drv_uart_tx("\r\n",2);
                    if( command_buffer_idx != 0){
                        command_buffer[command_buffer_idx] = 0;
                        ps_publish(out_topic, command_buffer, command_buffer_idx);
                        memset(command_buffer, COMMAND_BUFFER_SIZE, 0);
                    }
                    command_buffer_idx = 0;
                    break;
                default:
                    command_buffer[command_buffer_idx++] = *echo_buffer;
                    if(command_buffer_idx >= COMMAND_BUFFER_SIZE){
                        command_buffer_idx = 0;
                    }
                    break;
            }
            //echo back to console
            if(is_ascii(*echo_buffer)){
                nrf_drv_uart_tx(echo_buffer, 1);
            }
            //accept next character
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
void os_uart_set_broadcast_topic(ps_topic_t topic){
    out_topic = topic;
}
