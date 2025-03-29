#include "mov.h"
#include "shell.h"
#include "sim.h"
#include "stdio.h"
#include "stdint.h"
#include "utils.h"

void execute_movz(uint32_t instruction) {
    uint32_t rd    =  instruction        & 0x1F;    
    uint32_t imm16 = (instruction >> 5)  & 0xFFFF; 
    uint32_t hw    = (instruction >> 21) & 0x3;   
    
    if (hw == 0) {
        NEXT_STATE.REGS[rd] = imm16;  
    }

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_l_sx(uint32_t instruction) {
    uint32_t rd    = instruction         & 0x1F;   
    uint32_t rn    = (instruction >> 5)  & 0x1F;   
    uint32_t imms  = (instruction >> 10) & 0x3F;  
    uint32_t shift = (instruction >> 16) & 0x3F; 


    uint64_t operand2;
    if (imms ==0b111111) {
        operand2 = CURRENT_STATE.REGS[rn] >> shift;
    }
    else if (imms != 0b111111) {
        operand2 = CURRENT_STATE.REGS[rn] <<64- shift;
    }
    NEXT_STATE.REGS[rd] = operand2;
           
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}