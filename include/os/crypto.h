#ifndef CRYPTO_H
#define CRYPTO_H
#include <stdint.h>

void    os_rand_add_entroy(uint32_t e);
uint32_t os_rand(void);

#endif
