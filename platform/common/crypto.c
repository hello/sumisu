#include "crypto.h"

static volatile uint32_t rand_state;
void    os_rand_add_entroy(uint32_t e){
    rand_state ^= e;
}
/**
 * using bsd lgc
 * TODO switch to mersenne twister or something better
 */
uint32_t os_rand(void){
    rand_state = (uint32_t)((1103515245 * (uint64_t)rand_state + 12345) % UINT32_MAX);
    return rand_state;
}

