#include "io.h"
#include "cmsis_os.h"
#include "pubsub.h"
static const char * const g_pcHex = "0123456789ABCDEF";
static volatile uint32_t viewtag = LOG_LEVEL_ALL;

typedef void (*out_func_t)(const char * str, int len, void * data);
static void _va_printf( va_list vaArgP, const char * pcString, out_func_t func, void * data );
//out_funcs
static void _os_puts(const char * str, int len, void * data);
static void _ps_puts(const char * str, int len, void * data);



void os_log(uint32_t loglevel, const char * format, ...){
    va_list args;
    va_start(args, format);

    if( loglevel & viewtag ){
        //todo protect print
        _va_printf(args, format, _os_puts, NULL);
    }
    if( loglevel & LOG_LEVEL_INFO){
        //TODO make this not hardcoded
        ps_topic_t temp = PS_UART0_TX;
        _va_printf(args, format, _ps_puts, &temp);
    }

    va_end(args);
}

uint32_t os_get_loglevel(void){
    return viewtag;
}

void os_set_loglevel(uint32_t loglevel){
    viewtag = loglevel;
}

static void _ps_puts(const char * str, int len, void * data){
    ps_topic_t * temp = (ps_topic_t *)data;
    ps_publish(*temp, str, len);
}
static void _os_puts(const char * str, int len, void * data){
    int i;
    for(i = 0; i < len; i ++){
        os_putc(str[i]);
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
