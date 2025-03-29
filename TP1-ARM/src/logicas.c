#include "logicas.h"
#include "shell.h"
#include "sim.h"
#include "stdio.h"
#include "stdint.h"
#include "utils.h"

void execute_HLT(uint32_t instruction) {

    RUN_BIT = 0;
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return;
}

void execute_CMP_inmediate(uint32_t instruction) {
    uint32_t rn    = (instruction >> 5)  & 0x1F;     
    uint32_t imm12 = (instruction >> 10) & 0xFFF; 
    uint32_t shift = (instruction >> 22) & 0x3;  

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;

    NEXT_STATE.FLAG_N = setFlagN(result); 
    NEXT_STATE.FLAG_Z = setFlagZ(result);      
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_CMP_extended_register(uint32_t instruction) {
    uint32_t rn    = (instruction >> 5)  & 0x1F;   
    uint32_t rm    = (instruction >> 16) & 0x1F; 
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;

    NEXT_STATE.FLAG_N = setFlagN(result);        
    NEXT_STATE.FLAG_Z = setFlagZ(result);             
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_ands_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0)  & 0x1F;   
    uint32_t rn = (instruction >> 5)  & 0x1F;  
    uint32_t rm = (instruction >> 16) & 0x1F;  

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] & operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = setFlagN(result);        
    NEXT_STATE.FLAG_Z = setFlagZ(result);             

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_eor_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0)  & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5)  & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] ^ operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_orr_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0)  & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5)  & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] | operand2;
    NEXT_STATE.REGS[rd] = result;

}