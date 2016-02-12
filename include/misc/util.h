#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>
#include <stdint.h>
#include "io.h"

/**
 * useful macros
 */
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define START_THREAD(NAME, pf, stack, arg) do{\
    osThreadDef_t t = (osThreadDef_t){\
        .name = NAME,\
        .pthread = pf,\
        .tpriority = 0,\
        .instances = 1,\
        .stacksize = stack,\
    };\
    osThreadCreate(&t, arg);\
}while(0)

#define END_THREAD() osThreadTerminate(osThreadGetId())

#define ASSERT_OK(res) do{\
    uint32_t result = res;\
    if ( osOK != result ){\
        LOGE("Failed Initialization %u\r\n",result);\
    }\
}while(0)

#define PRINT_HEX(ptr, sz) do{\
    int i;\
    for(i = 0; i < sz; i++){\
        LOGI("%x ", ((uint8_t*)ptr)[i]);\
    }\
    LOGI("\r\n");\
}while(0)


/**
 * ascii stuff
 */
bool is_ascii(uint8_t c);

#endif
