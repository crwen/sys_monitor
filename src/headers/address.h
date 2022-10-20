// include guards to prevent double declaration of any identifiers 
// such as types, enums and static variables
#ifndef ADDRESS_GUARD
#define ADDRESS_GUARD

#include <stdint.h>

/*
    S = 2^s, s = 6
    B = 2^b, b = 6
    E is defined in sram.c, 8
    For correctness verification, E can be 1, 2, 8, 1024
 */

#ifndef CACHE_SIMULATION_VERIFICATION
/*  for cache simulator verification
    use the marcos passed in
 */
#define SRAM_CACHE_INDEX_LENGTH (6)
#define SRAM_CACHE_OFFSET_LENGTH (6)
#define SRAM_CACHE_TAG_LENGTH (4)
#endif

#define PHYSICAL_PAGE_OFFSET_LENGTH (12)
#define PHYSICAL_PAGE_NUMBER_LENGTH (4)
#define PHYSICAL_ADDRESS_LENGTH (16)

#define VIRTUAL_PAGE_OFFSET_LENGTH (12)
#define VIRTUAL_PAGE_NUMBER_LENGTH (9) // 9 + 9 + 9 + 9
#define VIRTUAL_ADDRESS_LENGTH (48)


typedef union {
    uint64_t address_value;

    // physical address: 16
    struct {
        union {
            uint64_t paddr_value : PHYSICAL_ADDRESS_LENGTH;
            struct {
                uint64_t ppo : PHYSICAL_PAGE_OFFSET_LENGTH;
                uint64_t ppn : PHYSICAL_PAGE_NUMBER_LENGTH;
            };
        };
    };

    // virtual address: 48
    struct {
        union {
            uint64_t vaddr_value : VIRTUAL_ADDRESS_LENGTH;
            struct {
                uint64_t vpo : VIRTUAL_PAGE_OFFSET_LENGTH;
                uint64_t vpn4 : VIRTUAL_PAGE_NUMBER_LENGTH;
                uint64_t vpn3 : VIRTUAL_PAGE_NUMBER_LENGTH;
                uint64_t vpn2 : VIRTUAL_PAGE_NUMBER_LENGTH;
                uint64_t vpn1 : VIRTUAL_PAGE_NUMBER_LENGTH;
            };
        };
    };
    
    // sram_cache: 16
    struct {
        uint64_t co : SRAM_CACHE_OFFSET_LENGTH;
        uint64_t ci : SRAM_CACHE_INDEX_LENGTH;
        uint64_t ct : SRAM_CACHE_TAG_LENGTH;
    };

} address_t;

#endif
