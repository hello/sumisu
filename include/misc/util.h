#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>
#include <stdint.h>

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

/**
 * ascii stuff
 */
bool is_ascii(uint8_t c);

#endif
