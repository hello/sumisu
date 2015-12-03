#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>
#include <stdint.h>

/**
 * useful macros
 */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define END_THREAD() osThreadTerminate(osThreadGetId())

/**
 * ascii stuff
 */
bool is_ascii(uint8_t c);

#endif
