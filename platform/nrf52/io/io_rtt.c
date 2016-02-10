/**
 * this file implements the io interface with RTT
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "io.h"
#include "nrf.h"
#include "bsp.h"
#include "util.h"
#include "nrf_drv_config.h"
#include "pubsub.h"

#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>

#define LOG_TERMINAL_NORMAL 0
#define LOG_TERMINAL_INPUT 0

static char buf_normal_up[BUFFER_SIZE_UP];
static char buf_down[BUFFER_SIZE_DOWN];

#define COMMAND_BUFFER_SIZE BUFFER_SIZE_DOWN
static uint8_t command_buffer[COMMAND_BUFFER_SIZE+1];
static uint32_t command_buffer_idx;
static ps_topic_t out_topic;

void os_putc(char c){
    SEGGER_RTT_Write( LOG_TERMINAL_NORMAL, &c, 1 );
}

static void _rtt_daemon(const void * arg){
    while(1){
        uint8_t inbuf[16] = {0};
        int i, bytes_read;
        if( (bytes_read = SEGGER_RTT_Read( LOG_TERMINAL_INPUT, inbuf, sizeof(inbuf))) ){
            for(i = 0; i < bytes_read; i++){
                uint8_t byte = inbuf[i];
                if (byte == '\n' ||  command_buffer_idx >= COMMAND_BUFFER_SIZE ){
                    ps_publish(out_topic, command_buffer, command_buffer_idx+1);
                    command_buffer_idx = 0;
                    memset(command_buffer, COMMAND_BUFFER_SIZE, 0);
                    break;
                }else if(byte == 0x08 || byte == 0x7f){
                    if(command_buffer_idx){
                        command_buffer[command_buffer_idx--] = 0;
                    }
                }else{
                    command_buffer[command_buffer_idx++] = byte;
                }
            }
        } else {
            osDelay(100);
        }
    }

    END_THREAD();
}

static uint32_t rtt_init(void)
{
    if (SEGGER_RTT_ConfigUpBuffer(LOG_TERMINAL_NORMAL,"OUT",buf_normal_up, BUFFER_SIZE_UP,SEGGER_RTT_MODE_NO_BLOCK_TRIM) != 0) {
        return NRF_ERROR_INVALID_STATE;
    }

    if (SEGGER_RTT_ConfigDownBuffer(LOG_TERMINAL_INPUT,"IN",buf_down,BUFFER_SIZE_DOWN,SEGGER_RTT_MODE_NO_BLOCK_SKIP) != 0) {
        return NRF_ERROR_INVALID_STATE;
    }
    return NRF_SUCCESS;
}

void os_io_init(void){
    rtt_init();
    
    osThreadDef_t t = (osThreadDef_t){
        .name = "rttd",
        .pthread = _rtt_daemon,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 256,
    };
    if(osThreadCreate(&t, NULL)){
        return osOK;
    }
}

void os_io_set_broadcast_topic(ps_topic_t topic){
    out_topic = topic;
}
