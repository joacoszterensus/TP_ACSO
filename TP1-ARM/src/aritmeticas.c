#include "aritmeticas.h"
#include"shell.h"
#include "sim.h"
#include "utils.h"

void execute_adds_inmediate(uint32_t instruction) {
    uint32_t rd    = (instruction >> 0)  & 0x1F;   
    uint32_t rn    = (instruction >> 5)  & 0x1F;   
    uint32_t imm12 = (instruction >> 10) & 0xFFF; 
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = setFlagN(result);        
    NEXT_STATE.FLAG_Z = setFlagZ(result);            
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_adds_extended_register(uint32_t instruction) {
    uint32_t rd    = (instruction >> 0)  & 0x1F;   
    uint32_t rn    = (instruction >> 5)  & 0x1F;   
    uint32_t rm    = (instruction >> 16) & 0x1F;  
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = setFlagN(result);        
    NEXT_STATE.FLAG_Z = setFlagZ(result);       
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_sub_inmediate(uint32_t instruction) {
    uint32_t rd    = (instruction >> 0)  & 0x1F;     
    uint32_t rn    = (instruction >> 5)  & 0x1F;      
    uint32_t imm12 = (instruction >> 10) & 0xFFF;     
    uint32_t shift = (instruction >> 22) & 0x3;       

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    operand2 = ~operand2;  
    int64_t result = CURRENT_STATE.REGS[rn] + operand2 + 1;  
    
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = setFlagN(result);        
    NEXT_STATE.FLAG_Z = setFlagZ(result);            

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_sub_extended_register(uint32_t instruction) {
    uint32_t rd    = (instruction >> 0) & 0x1F;      
    uint32_t rn    = (instruction >> 5) & 0x1F;      
    uint32_t rm    = (instruction >> 16) & 0x1F;     
    uint32_t shift = (instruction >> 22) & 0x3;      

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    operand2 = ~operand2;  
    int64_t result = CURRENT_STATE.REGS[rn] + operand2 + 1;  
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = setFlagN(result);        
    NEXT_STATE.FLAG_Z = setFlagZ(result);            

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_add_inmediate(uint32_t instruction) {
    uint32_t rd    = (instruction >> 0)  & 0x1F;   
    uint32_t rn    = (instruction >> 5)  & 0x1F;   
    uint32_t imm12 = (instruction >> 10) & 0xFFF; 
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;       
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_add_extended_register(uint32_t instruction) {
    uint32_t rd    = (instruction >> 0)  & 0x1F;   
    uint32_t rn    = (instruction >> 5)  & 0x1F;   
    uint32_t rm    = (instruction >> 16) & 0x1F;  
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;
     
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_mul(uint32_t instruction) {
    uint32_t rd = (instruction >> 0)  & 0x1F;   
    uint32_t rn = (instruction >> 5)  & 0x1F;   
    uint32_t rm = (instruction >> 16) & 0x1F;  

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] * operand2;

    NEXT_STATE.REGS[rd] = result;
     
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
