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

#define END_THREAD() osThreadTerminate(osThreadGetId())

/**
 * ascii stuff
 */
bool is_ascii(uint8_t c);

#endif
