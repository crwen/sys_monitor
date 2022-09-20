#include <stdio.h>
#include <stdint.h>
#include "hardware/cpu/register.h"
#include "hardware/memory/instruction.h"
#include "hardware/memory/dram.h"
#include "hardware/cpu/mmu.h"


static uint64_t decode_od(od_t od) {
    if (od.type == IMM) {
        return *((uint64_t *)&od.imm);
    } else if (od.type == REG) {
        // reg.rsp 0xabcd, 1     pmm[0xabcd] = 1
        // return oxabcd
        return (uint64_t)od.reg1;
    } else {
        // mm
        uint64_t vaddr = 0;

        if (od.type == MM_IMM) {
            vaddr = od.imm;
        } else if (od.type == MM_REG) {
            // store reg
            vaddr = *(od.reg1);
        } else if (od.type == MM_IMM_REG) {
            vaddr = od.imm + *(od.reg1);
        } else if (od.type == MM_REG1_REG2) {
            vaddr = *(od.reg1) + *(od.reg2);
        } else if (od.type == MM_IMM_REG1_REG2) {
            vaddr = od.imm + *(od.reg1) + *(od.reg2);
        } else if (od.type == MM_REG2_S) {
            vaddr = od.scal * (*(od.reg2));
        } else if (od.type == MM_IMM_REG2_S) {
            vaddr = od.imm + od.scal * (*(od.reg2));
        } else if (od.type == MM_REG1_REG2_S) {
            vaddr = *(od.reg1) + od.scal * (*(od.reg2)); } else if (od.type == MM_IMM_REG1_REG2_S) {
            vaddr = od.imm +  *(od.reg1) + od.scal * (*(od.reg2));
        }

        return va2pa(vaddr);
    }
}

void instruction_cycle() {
    // fetch
    inst_t *instr = (inst_t*)reg.rip;
    // decode
    uint64_t src = decode_od(instr->src);
    uint64_t dst = decode_od(instr->dst);

    handler_t handler = handler_table[instr->op];

    // execute
    handler(src, dst);

    printf("    %s\n", instr->code);
}

void init_handler_table() {
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[mov_reg_mem] = &mov_reg_mem_handler;
    handler_table[mov_mem_reg] = &mov_mem_reg_handler;
    handler_table[push_reg] = &push_reg_handler;
    handler_table[pop_reg] = &pop_reg_handler;
    handler_table[call] = &call_handler;
    handler_table[ret] = &ret_handler;
    handler_table[add_reg_reg] = &add_reg_reg_handler;
}

void mov_reg_reg_handler(uint64_t src, uint64_t dst) {
    *(uint64_t *)dst = *(uint64_t *)src;
    reg.rip = reg.rip + sizeof(inst_t);
}

void mov_reg_mem_handler(uint64_t src, uint64_t dst) {
    write64bits_dram(va2pa(dst), *(uint64_t *)src);
    reg.rip = reg.rip + sizeof(inst_t);
}

void mov_mem_reg_handler(uint64_t src, uint64_t dst) {
    *(uint64_t *)dst = read64bits_dram(va2pa(src));
    reg.rip = reg.rip + sizeof(inst_t);
}

void add_reg_reg_handler(uint64_t src, uint64_t dst) {
    *(uint64_t *)dst = *(uint64_t *)dst + *(uint64_t *)src;
    reg.rip = reg.rip + sizeof(inst_t);
}

void push_reg_handler(uint64_t src, uint64_t dst) {
    // rsp - 8
    reg.rsp = reg.rsp - 8;
    write64bits_dram(va2pa(reg.rsp), *(uint64_t *)src);
    reg.rip = reg.rip + sizeof(inst_t);
}

void pop_reg_handler(uint64_t src, uint64_t dst) {
    // write to register
    *(uint64_t *)src = read64bits_dram(va2pa(reg.rsp));
    // rsp + 8
    reg.rsp = reg.rsp + 8;
    reg.rip = reg.rip + sizeof(inst_t);
}

void call_handler(uint64_t src, uint64_t dst) {
    // rsp - 8
    reg.rsp = reg.rsp - 8;
    // write return address to rsp memory
    write64bits_dram(va2pa(reg.rsp), reg.rip + sizeof(inst_t));
    // jmp 
    reg.rip = src;
}

void ret_handler(uint64_t src, uint64_t dst) {
    // reg.rbp = read64bits_dram(va2pa(reg.rbp));
    reg.rip = read64bits_dram(va2pa(reg.rsp));
    reg.rsp = reg.rsp + 8;
}