#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_drv_uart.h"
#include "util.h"
#include "app_fifo.h"
#include "nrf_drv_config.h"

typedef void (*out_func_t)(const char * str, int len, void * data);
static const char * const g_pcHex = "0123456789ABCDEF";

static uint8_t echo_buffer[1];
static uint8_t output_buffer[1];
static app_fifo_t out;

static void _va_printf( va_list vaArgP, const char * pcString, out_func_t func, void * data );
static void _uart_event_handler(nrf_drv_uart_event_t * p_event, void * p_context);
static void _uart_puts(const char * str, int len, void * data);

static void _uart_puts(const char * str, int len, void * data){
    int i;
    for(i = 0; i < len; i++){
        app_fifo_put(&out, str[i]);
    }
    //hack to trigger uart
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

static void _va_printf( va_list vaArgP, const char * pcString, out_func_t func, void * data ) {
    unsigned long ulIdx, ulValue, ulPos, ulCount, ulBase, ulNeg;
    char *pcStr, pcBuf[16], cFill;

    while(*pcString)
    {
        for(ulIdx = 0; (pcString[ulIdx] != '%') && (pcString[ulIdx] != '\0');
                ulIdx++)
        {
        }
        func(pcString, ulIdx, data);
        pcString += ulIdx;
        if(*pcString == '%')
        {
            pcString++;
            ulCount = 0;
            cFill = ' ';
again:
            switch(*pcString++)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    {
                        if((pcString[-1] == '0') && (ulCount == 0))
                        {
                            cFill = '0';
                        }
                        ulCount *= 10;
                        ulCount += pcString[-1] - '0';

                        goto again;
                    }
                case 'c':
                    {
                        ulValue = va_arg(vaArgP, unsigned long);

                        func((char *)&ulValue, 1, data);

                        break;
                    }
                case 'd':
                    {
                        ulValue = va_arg(vaArgP, unsigned long);
                        ulPos = 0;
                        if((long)ulValue < 0)
                        {
                            ulValue = -(long)ulValue;
                            ulNeg = 1;
                        }
                        else
                        {
                            ulNeg = 0;
                        }

                        ulBase = 10;
                        goto convert;
                    }

                case 's':
                    {
                        pcStr = va_arg(vaArgP, char *);
                        for(ulIdx = 0; pcStr[ulIdx] != '\0'; ulIdx++)
                        {
                        }
                        func(pcStr, ulIdx, data);
                        if(ulCount > ulIdx)
                        {
                            ulCount -= ulIdx;
                            while(ulCount--)
                            {
                                func(" ", 1, data);
                            }
                        }
                        break;
                    }
                case 'u':
                    {
                        ulValue = va_arg(vaArgP, unsigned long);
                        ulPos = 0;
                        ulBase = 10;
                        ulNeg = 0;
                        goto convert;
                    }
                case 'x':
                case 'X':
                case 'p':
                    {
                        ulValue = va_arg(vaArgP, unsigned long);
                        ulPos = 0;
                        ulBase = 16;
                        ulNeg = 0;
convert:
                        for(ulIdx = 1;
                                (((ulIdx * ulBase) <= ulValue) &&
                                 (((ulIdx * ulBase) / ulBase) == ulIdx));
                                ulIdx *= ulBase, ulCount--)
                        {
                        }
                        if(ulNeg)
                        {
                            ulCount--;
                        }

                        if(ulNeg && (cFill == '0'))
                        {

                            pcBuf[ulPos++] = '-';
                            ulNeg = 0;
                        }

                        if((ulCount > 1) && (ulCount < 16))
                        {
                            for(ulCount--; ulCount; ulCount--)
                            {
                                pcBuf[ulPos++] = cFill;
                            }
                        }
                        if(ulNeg)
                        {
                            pcBuf[ulPos++] = '-';
                        }

                        for(; ulIdx; ulIdx /= ulBase)
                        {
                            pcBuf[ulPos++] = g_pcHex[(ulValue / ulIdx) % ulBase];
                        }
                        func(pcBuf, ulPos, data);

                        break;
                    }
                case '%':
                    {
                        func(pcString - 1, 1, data);

                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }
    }

}

void os_printf(const char * format, ...){
    char * itr = format;
    va_list args;
    va_start(args, format);
    _va_printf(args, format, _uart_puts, NULL);
    va_end(args);
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
