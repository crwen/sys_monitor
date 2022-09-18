#ifndef CPU_GUARD
#define CPU_GUARD

#include <stdint.h>

typedef struct REG_STRUCT {

    union {
        uint64_t rax;
        uint32_t eax;
        uint16_t ax;
        struct {
            uint8_t al;
            uint8_t ah;
        };
    };
    union {
        uint64_t rbx;
        uint32_t ebx;
        uint16_t bx;
        struct {
            uint8_t bl;
            uint8_t bh; }; };
    union {
        uint64_t rcx;
        uint32_t ecx;
        uint16_t cx;
        struct {
            uint8_t cl;
            uint8_t ch;
        };
    };
    union {
        uint64_t rdx;
        uint32_t edx;
        uint16_t dx;
        struct {
            uint8_t dl;
            uint8_t dh;
        };
    };
    union {
        uint64_t rsi;
        uint32_t esi;
        uint16_t si;
        struct {
            uint8_t sil;
            uint8_t sih;
        };
    };
    union {
        uint64_t rdi;
        uint32_t edi;
        uint16_t di;
        struct {
            uint8_t dil;
            uint8_t dih;
        };
    };
    union {
        uint64_t rbp;
        uint32_t ebp;
        uint16_t bp;
        struct {
            uint8_t bpl;
            uint8_t bph;
        };
    };
    union {
        uint64_t rsp;
        uint32_t esp;
        uint16_t sp;
        struct {
            uint8_t spl;
            uint8_t sph;
        };
    };

    uint64_t rip;
} reg_t;

reg_t reg;


#endif