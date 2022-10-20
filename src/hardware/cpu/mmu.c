// Memory Management Unit 
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>
#include<headers/address.h>

uint64_t va2pa(uint64_t vaddr, core_t *cr) {
    return vaddr % PHYSICAL_MEMORY_SPACE;
}


static uint64_t page_walk(uint64_t vaddr_value) {
    address_t vaddr = { .vaddr_value = vaddr_value };

    // CR3 register's value is malloced on the heap of the simulator
    pte123_t *pgd = (pte123_t *)cpu_controls.cr3;
    pte123_t *pud = (pte123_t *)(pgd[vaddr.vpn1].paddr);
}