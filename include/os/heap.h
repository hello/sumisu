#ifndef HEAP_H
#define HEAP_H
#include <stddef.h>

void * os_malloc(size_t sz);
void os_free(void * ptr);
size_t os_free_heap_size(void);

#endif
