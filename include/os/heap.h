#ifndef HEAP_H
#define HEAP_H
#include <stddef.h>

void * os_malloc(size_t sz);
void os_free(void * ptr);

#endif
