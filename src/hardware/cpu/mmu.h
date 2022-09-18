#ifndef MMU_GUARD
#define MMU_GUARD
// memory management unit

#include <stdint.h>

uint64_t va2pa(uint64_t va);

#endif