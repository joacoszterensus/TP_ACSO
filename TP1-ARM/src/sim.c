
#include <stdio.h>
#include <stdint.h>
#include "shell.h"

void execute_adds_inmediate(uint32_t instruction);
void execute_adds_extended_register(uint32_t instruction);
void execute_sub_inmediate(uint32_t instruction);
void execute_sub_extended_register(uint32_t instruction);
void execute_HLT(uint32_t instruction);
void execute_CMP_immediate(uint32_t instruction);
void execute_CMP_extended_register(uint32_t instruction);
void execute_ands_shifted_register(uint32_t instruction);

typedef void (*InstructionFunction)(uint32_t instruction);

typedef struct {
    uint32_t opcode;
    uint32_t mask;
    InstructionFunction execute;
} InstructionEntry;

InstructionEntry instruction_set[] = {
    {0b10110001     << 24, 0xFF000000, execute_adds_inmediate},        //B1
    {0b101010110000 << 20, 0xFFF00000, execute_adds_extended_register},//AB0
    {0b11010001     << 24, 0xFF000000, execute_sub_inmediate},         //D1
    {0b111010110000 << 20, 0xFFF00000, execute_sub_extended_register}, //EB0
    {0b110101000100 << 20, 0xFFF00000, execute_HLT},                   //D44
    {0b111010110010 << 20, 0xFFF00000, execute_CMP_extended_register}, //EB1
    {0b11110001     << 24, 0xFF000000, execute_CMP_immediate},         //F1
    {0b10001010     << 24, 0xFF000000, execute_ands_shifted_register}, //8A
    {0, 0, NULL} 
};

void process_instruction() {
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    printf("instruction: 0x%08X\n", instruction);
    for (int i = 0; instruction_set[i].execute != NULL; i++) {
        if ((instruction & instruction_set[i].mask) == instruction_set[i].opcode) {
            instruction_set[i].execute(instruction);
            printf("Executing instruction: 0x%08X\n", instruction);
            break;
        }
    }
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

// uint32_t decode_instruction(uint32_t instruction){

// }

void execute_adds_inmediate(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t imm12 = (instruction >> 10) & 0xFFF; // Bits 10-21: imm12
    uint32_t shift = (instruction >> 22) & 0x3; // Bits 22-23: shift

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  // LSL #12
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        // Negative
    NEXT_STATE.FLAG_Z = (result == 0);             // Zero
    
}

void execute_adds_extended_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm
    uint32_t shift = (instruction >> 22) & 0x3; // Bits 22-23: shift

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  // LSL #12
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        // Negative
    NEXT_STATE.FLAG_Z = (result == 0);             // Zero
}

void execute_sub_inmediate(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t imm12 = (instruction >> 10) & 0xFFF; // Bits 10-21: imm12
    uint32_t shift = (instruction >> 22) & 0x3; // Bits 22-23: shift

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  // LSL #12
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        // Negative
    NEXT_STATE.FLAG_Z = (result == 0);             // Zero
}

void execute_sub_extended_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm
    uint32_t shift = (instruction >> 22) & 0x3; // Bits 22-23: shift

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  // LSL #12
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        // Negative
    NEXT_STATE.FLAG_Z = (result == 0);             // Zero
}

void execute_HLT(uint32_t instruction) {

    RUN_BIT = 0;
    return;
}

void execute_CMP_immediate(uint32_t instruction) {
    uint32_t rn = (instruction >> 5) & 0x1F;     // Bits 5-9: Rn (registro fuente)
    uint32_t imm12 = (instruction >> 10) & 0xFFF; // Bits 10-21: imm12
    uint32_t shift = (instruction >> 22) & 0x3;  // Bits 22-23: shift

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  // LSL #12
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;

    // Actualizar flags de condición
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // Flag de signo (negativo)
    NEXT_STATE.FLAG_Z = (result == 0);      // Flag de cero
}

void execute_CMP_extended_register(uint32_t instruction) {
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm
    uint32_t shift = (instruction >> 22) & 0x3; // Bits 22-23: shift

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  // LSL #12
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        // Negative
    NEXT_STATE.FLAG_Z = (result == 0);             // Zero
}

void execute_ands_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    

    uint64_t result = CURRENT_STATE.REGS[rn] & operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        // Negative
    NEXT_STATE.FLAG_Z = (result == 0);             // Zero
}