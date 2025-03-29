#include "memoria.h"
#include "shell.h"
#include "sim.h"
#include "stdio.h"
#include "stdint.h"
#include "utils.h"


void execute_stur(uint32_t instruction) {
    uint32_t rt = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    int32_t imm9 = (instruction >> 12) & 0x1FF; 
    
    imm9 = signextend64(imm9, 9);  
    
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    
    mem_write_32(address, CURRENT_STATE.REGS[rt]);
    
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_stur_b(uint32_t instruction) {
    uint32_t rt = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    int32_t imm9 = (instruction >> 12) & 0x1FF; 
    
    imm9 = signextend64(imm9, 9);
    
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    
    uint32_t byte_to_store = CURRENT_STATE.REGS[rt] & 0xFF;
    
    mem_write_32(address, byte_to_store);
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_stur_h(uint32_t instruction) {
    uint32_t rt = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    int32_t imm9 = (instruction >> 12) & 0x1FF; 
    
    imm9 = signextend64(imm9, 9);
    
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    
    uint32_t byte_to_store = CURRENT_STATE.REGS[rt] & 0xFFFF;
    
    mem_write_32(address, byte_to_store);
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_ldur(uint32_t instruction) {
    uint32_t rt = instruction & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    int64_t imm9 = (instruction >> 12) & 0x1FF;
    uint8_t size = (instruction >> 30) & 0x3;

    imm9 = signextend64(imm9, 9);
    
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    
    uint32_t low = mem_read_32(address);
    uint32_t high = mem_read_32(address + 4);
    
    uint64_t data = ((uint64_t) high << 32) | low;
    
    NEXT_STATE.REGS[rt] = data;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_ldur_h(uint32_t instruction) {
    uint32_t rt = instruction & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    int64_t imm9 = (instruction >> 12) & 0x1FF;
    uint8_t size = (instruction >> 30) & 0x3;

    imm9 = signextend64(imm9, 9);
    
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    
    uint32_t low = mem_read_32(address);
    uint32_t high = mem_read_32(address + 4);
    
    uint64_t data = ((uint64_t) high << 32) | low;
    
    NEXT_STATE.REGS[rt] = data & 0xFFFF;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_ldur_b(uint32_t instruction) {
    uint32_t rt = instruction & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    int64_t imm9 = (instruction >> 12) & 0x1FF;
    uint8_t size = (instruction >> 30) & 0x3;

    imm9 = signextend64(imm9, 9);
    
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    
    uint32_t low = mem_read_32(address);
    uint32_t high = mem_read_32(address + 4);
    
    uint64_t data = ((uint64_t) high << 32) | low;
    
    NEXT_STATE.REGS[rt] = data & 0xFF;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}