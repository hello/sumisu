#ifndef IO_H
#define IO_H
#include <stdarg.h>

void os_uart_init(void);
void os_printf(const char * format, ...);

#endif
