#include <stdio.h>
#include "hardware/cpu/register.h"
#include "hardware/cpu/mmu.h"
#include "hardware/memory/instruction.h"
#include "hardware/memory/dram.h"
#include "hardware/disk/elf.h"

int main() {

    init_handler_table();

    reg.rax = 0x12340000;
    reg.rbx = 0x0;
    reg.rcx = 0x800660;
    reg.rdx = 0xabcd;
    reg.rsi = 0x7ffffffee2f8;
    reg.rdi = 0x1;
    reg.rbp = 0x7ffffffee210;
    reg.rsp = 0x7ffffffee1f0;

    reg.rip = (uint64_t)&program[11];

    write64bits_dram(va2pa(0x7ffffffee210), 0x08000660);  // rbp
    write64bits_dram(va2pa(0x7ffffffee208), 0x0);
    write64bits_dram(va2pa(0x7ffffffee200), 0xabcd);
    write64bits_dram(va2pa(0x7ffffffee1f8), 0x12340000);
    write64bits_dram(va2pa(0x7ffffffee1f0), 0x08000660); // rsp

    uint64_t pa = va2pa(0x7ffffffee210);
    printf("%16llx\n", *((uint64_t *)(&mm[pa])));

    print_register();
    print_stack();
    // run inst
    for (int i = 0; i < 15; i ++) {
        // instruction_cycle();
    }


    // verify

    int match = 1;

    match = match && (reg.rax == 0x1234abcd);
    match = match && (reg.rbx == 0x0);
    match = match && (reg.rcx == 0x08000600);
    match = match && (reg.rdx == 0x12340000);
    match = match && (reg.rsi == 0xabcd);
    match = match && (reg.rdi == 0x12340000);
    match = match && (reg.rbp == 0x7ffffffee210);
    match = match && (reg.rsp == 0x7ffffffee1f0);

    if (match) {
        printf("register match\n");
    } else {
        printf("register not match\n");
    }

    match = match && (read64bits_dram( va2pa(0x7ffffffee210)) == 0x08000660); // rbp
    match = match && (read64bits_dram( va2pa(0x7ffffffee208)) == 0x1234abcd);        // 
    match = match && (read64bits_dram( va2pa(0x7ffffffee200)) == 0xabcd);
    match = match && (read64bits_dram( va2pa(0x7ffffffee1f8)) == 0x12340000);
    match = match && (read64bits_dram( va2pa(0x7ffffffee1f0)) == 0x08000660);// rspA

    printf("%16llx\n", read64bits_dram( va2pa(0x7ffffffee210)));
    if (match) {
        printf("memory match\n");
    } else {
        printf("memory not match\n");
    }

    return 0;
}
