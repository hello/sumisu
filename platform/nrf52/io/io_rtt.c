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

#include "rtt.h"

#define LOG_TERMINAL 0

#define COMMAND_BUFFER_SIZE BUFFER_SIZE_DOWN
static uint8_t command_buffer[COMMAND_BUFFER_SIZE+1];
static uint32_t command_buffer_idx;
static ps_topic_t out_topic;

void os_putc(char c){
    SEGGER_RTT_Write( LOG_TERMINAL, &c, 1 );
}

static void _rtt_daemon(const void * arg){
    while(1){
        memset(command_buffer, COMMAND_BUFFER_SIZE, 0);
        command_buffer_idx = SEGGER_RTT_Read( LOG_TERMINAL, &command_buffer, COMMAND_BUFFER_SIZE );
        if( command_buffer_idx > 0 ) {
            ps_publish(out_topic, command_buffer, command_buffer_idx+1);
        } else {
            osDelay(100);
        }
    }

    END_THREAD();
}

static void _rtt_init(void){
    RTT_INIT_UP_CH(LOG_TERMINAL, "OUT", BUFFER_SIZE_UP, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    RTT_INIT_DOWN_CH(LOG_TERMINAL, "IN", BUFFER_SIZE_DOWN, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    osThreadDef_t t = (osThreadDef_t){
        .name = "rttd",
        .pthread = _rtt_daemon,
        .tpriority = 2,
        .instances = 1,
        .stacksize = 256,
    };
    osThreadCreate(&t, NULL);/*TODO assert*/
}

void os_io_init(void){
    _rtt_init();
}
void os_io_set_broadcast_topic(ps_topic_t topic){
    out_topic = topic;
}
