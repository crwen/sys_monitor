#include <stdint.h>
#include "hardware/cpu/mmu.h"
#include "hardware/memory/dram.h"


uint64_t va2pa(uint64_t vaddr) {
    return vaddr % MM_LEN;
}