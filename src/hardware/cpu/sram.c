
#include "headers/address.h"
#include "headers/memory.h"
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

            // TODO: upate replacer according to replacement policy
            return line.block[paddr.CO];
        }
    }
    
    // cache miss, load from memory
    // TODO: update LRU
    // TODO: select one victim by replacement policy if set is full
    // update line state
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

            // write-through and no write allocate
            // write data block to memory
            uint64_t base = (paddr.paddr_value >> SRAM_CACHE_OFFSET_LENGTH) << SRAM_CACHE_OFFSET_LENGTH;
            int line_num = (1 >> SRAM_CACHE_OFFSET_LENGTH) / sizeof(uint64_t);

            for (int j = 0; j < line_num; j ++) {
                write64bits_dram(base + sizeof(uint64_t) * j, *(uint64_t *)&(line.block[sizeof(uint64_t) * j]));
            }

            // TODO: upate replacer according to replacement policy

            return ;
        }
    }
    // cache miss, load from memory
    // TODO: upate replacer
    // TODO: select one victim by replacement policy if set is full

    // update line state
}
