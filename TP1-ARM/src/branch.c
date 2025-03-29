#include "branch.h"
#include "shell.h"
#include "sim.h"
#include "stdio.h"  
#include "stdint.h"
#include "utils.h"

InstructionEntry b_cond_set[]= {
    {0b0000 , 0xF, execute_b_eq},
    {0b0001 , 0xF, execute_b_ne},   
    {0b1100 , 0xF, execute_b_gt},   
    {0b1011 , 0xF, execute_b_lt},   
    {0b1010 , 0xF, execute_b_ge},   
    {0b1101 , 0xF, execute_b_le},
                  
    {0, 0, NULL} 
};

void execute_b_conditional(uint32_t instruction) {
    uint32_t cond = (instruction) & 0xF; 
    
    for (int i = 0; b_cond_set[i].execute != NULL; i++) {
        if ((cond & b_cond_set[i].mask) == b_cond_set[i].opcode) {
            b_cond_set[i].execute(instruction);
            break;
        }
    }
}

void execute_b_target(uint32_t instruction) {
    int64_t imm26 = instruction & 0x03FFFFFF;
    int64_t offset = signextend64(imm26, 26) << 2;
    
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}

void execute_br(uint32_t instruction) {
    uint32_t rn = (instruction >> 5) & 0x1F;  
    NEXT_STATE.PC = CURRENT_STATE.REGS[rn];   
}

void execute_b_eq(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_Z) {
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        int64_t offset = signextend64(imm19, 19) << 2;
        
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;     
    }
    else{
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void execute_b_ne(uint32_t instruction) {
    if (!CURRENT_STATE.FLAG_Z) { 
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        int64_t offset = signextend64(imm19, 19) << 2;
        
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;    
    }
    else{
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void execute_b_gt(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_Z==0 && CURRENT_STATE.FLAG_N==0) { 
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        int64_t offset = signextend64(imm19, 19) << 2;
        
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;    
    }
    else{
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void execute_b_lt(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_N) { 
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        int64_t offset = signextend64(imm19, 19) << 2;
        
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;    
    }
    else{
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void execute_b_ge(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_N==0) { 
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        int64_t offset = signextend64(imm19, 19) << 2;
        
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;    
    }
    else{
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void execute_b_le(uint32_t instruction) {
    if ( ! (CURRENT_STATE.FLAG_Z==0 && CURRENT_STATE.FLAG_N==0)) { 
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        int64_t offset = signextend64(imm19, 19) << 2;
        
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;    
    }
    else{
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void execute_cbz(uint32_t instruction) {
    uint32_t rt = (instruction >> 0) & 0x1F;   
    int32_t imm19 = (instruction >> 5) & 0x7FFFF; 
    int64_t offset = signextend64(imm19, 19) << 2;
    
    if (CURRENT_STATE.REGS[rt] == 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void execute_cbnz(uint32_t instruction) {
    uint32_t rt = (instruction >> 0) & 0x1F;   
    int32_t imm19 = (instruction >> 5) & 0x7FFFF; 
    
    int64_t offset = signextend64(imm19, 19) << 2;
    
    if (CURRENT_STATE.REGS[rt] != 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
