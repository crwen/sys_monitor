#ifndef SRAM_GUARD
#define SRAM_GUARD

#include "hardware/headers/address.h"
#include <stdint.h>

#define NUM_CACHE_LINE_PER_SET (8)

// write-back and write-allocate
typedef enum {
    CACHE_LINE_INVALID,
    CACHE_LINE_CLEAN,
    CACHE_LINE_DIRTY
} sram_cacheline_state_t;

// write-through and no-write-allocate
typedef enum {
    CACHE_LINE_INVALID2,
    CACHE_LINE_VALID,
} sram_cacheline_state_t2;


/**
 *           cache line
 * +-------------------------------------+ 
 * | state |   tag   |       block       | 
 * +-------------------------------------+ 
 **/
typedef struct {
    sram_cacheline_state_t state;
    uint64_t tag;
    uint8_t block[(1 << SRAM_CACHE_OFFSET_LENGTH)];
} sram_cacheline_t;

// cache set
typedef struct {
    sram_cacheline_t lines[NUM_CACHE_LINE_PER_SET];
} sram_cacheset_t;

typedef struct {
    sram_cacheset_t sets[(1 << SRAM_CACHE_INDEX_LENGTH)];
} sram_cache_t;

static sram_cache_t cache;

// read a byte at paddr
uint8_t sram_cache_read(address_t paddr) {
    sram_cacheset_t set = cache.sets[paddr.CI];
    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; ++ i) {
        sram_cacheline_t line = set.lines[i];
        if (line.state != CACHE_LINE_INVALID && line.tag == paddr.CT) {
            // cache hit
            return line.block[paddr.CO];
        }

        // cache miss, load from memory
        // TODO: upate LRU
        // TODO: select one victim by replacement policy if set is full
    }
    return 0;
}

// write a byte to paddr
// TODO
void sram_cache_write(address_t paddr, uint8_t data) {
    sram_cacheset_t set = cache.sets[paddr.CI];
    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; ++ i) {
        sram_cacheline_t line = set.lines[i];
        if (line.state != CACHE_LINE_INVALID && line.tag == paddr.CT) {
            // cache hit
            line.block[paddr.CO] = data;
            // TODO write to memory
            return ;
        }

        // cache miss, load from memory
        // TODO: upate LRU
        // TODO: select one victim by replacement policy if set is full
    }
}

#endif
