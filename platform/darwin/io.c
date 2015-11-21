#include "io.h"
#include <stdio.h>

void os_printf(const char * fmt, ...){
    va_list args;
    va_start(args, fmt);
    printf(fmt, args);
    va_end(args);
}
