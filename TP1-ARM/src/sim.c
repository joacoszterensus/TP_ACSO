
#include <stdio.h>
#include <stdint.h>
#include "shell.h"

void execute_adds_inmediate(uint32_t instruction);
void execute_adds_extended_register(uint32_t instruction);
void execute_sub_inmediate(uint32_t instruction);
void execute_sub_extended_register(uint32_t instruction);
void execute_HLT(uint32_t instruction);
void execute_CMP_inmediate(uint32_t instruction);
void execute_CMP_extended_register(uint32_t instruction);
void execute_ands_shifted_register(uint32_t instruction);
void execute_eor_shifted_register(uint32_t instruction);
void execute_orr_shifted_register(uint32_t instruction);
void execute_b_target(uint32_t instruction);
void execute_br(uint32_t instruction);
void execute_b_conditional(uint32_t instruction);
void execute_b_eq(uint32_t instruction);
void execute_b_ne(uint32_t instruction);
void execute_b_gt(uint32_t instruction);
void execute_b_lt(uint32_t instruction);
void execute_b_ge(uint32_t instruction);
void execute_b_le(uint32_t instruction);


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
    {0b11110001     << 24, 0xFF000000, execute_CMP_inmediate},         //F1
    {0b11101010     << 24, 0xFF000000, execute_ands_shifted_register}, //EA
    {0b11001010     << 24, 0xFF000000, execute_eor_shifted_register},  //CA
    {0b10101010     << 24, 0xFF000000, execute_orr_shifted_register},  //AA
    {0b000101       << 26, 0xFC000000, execute_b_target},              //03
    {0b1101011000011111000000<< 10, 0xFFFFFC00, execute_br},   
    {0b01010100     << 24, 0xFF000000, execute_b_conditional},         //54  
    
                  
    {0, 0, NULL} 
};

InstructionEntry b_cond_set[]= {
    {0b0000 , 0xF, execute_b_eq},
    {0b0001 , 0xF, execute_b_ne},   
    {0b1100 , 0xF, execute_b_gt},   
    {0b1011 , 0xF, execute_b_lt},   
    {0b1010 , 0xF, execute_b_ge},   
    {0b1101 , 0xF, execute_b_le},
                  
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

    int64_t result = CURRENT_STATE.REGS[rn] - operand2;
    NEXT_STATE.REGS[rd] = -result;

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

    int64_t result = CURRENT_STATE.REGS[rn] - operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        // Negative
    NEXT_STATE.FLAG_Z = (result == 0);             // Zero
}

void execute_HLT(uint32_t instruction) {

    RUN_BIT = 0;
    return;
}

void execute_CMP_inmediate(uint32_t instruction) {
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

void execute_eor_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] ^ operand2;
    NEXT_STATE.REGS[rd] = result;

}

void execute_orr_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] | operand2;
    NEXT_STATE.REGS[rd] = result;

}

void execute_b_target(uint32_t instruction) {
    // Extraer imm26 (bits 0-25)
    int64_t imm26 = instruction & 0x03FFFFFF;
    
    // Sign-extend a 64 bits y agregar '00' de manera segura
    int64_t offset = (imm26 << 38) >> 36;  // Corrección: primero castear a int64_t
    
    // Versión alternativa más explícita:
    // int64_t offset = ((int64_t)imm26 << 38) >> 36;
    
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    printf("B branch to: 0x%016lx\n", NEXT_STATE.PC);
}

void execute_br(uint32_t instruction) {
    uint32_t rn = (instruction >> 5) & 0x1F;  // Extraer bits 5-9 (Rn)
    NEXT_STATE.PC = CURRENT_STATE.REGS[rn];   // Saltar a la dirección en el registro
}

void execute_b_conditional(uint32_t instruction) {
    uint32_t cond = (instruction) & 0xF; //condition code
    
    for (int i = 0; b_cond_set[i].execute != NULL; i++) {
        if ((cond & b_cond_set[i].mask) == b_cond_set[i].opcode) {
            b_cond_set[i].execute(instruction);
            break;
        }
    }
}


void execute_b_eq(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_Z) {
        printf("BEQ instruction: 0x%08X\n", instruction);
        
        // 1. Extraer imm19 (bits 5-23) y extender signo
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        printf("imm19 raw: 0x%05X\n", imm19);
        
        // 2. Convertir a offset de bytes (imm19*4) con extensión de signo
        int64_t offset = signextend64(imm19, 19) << 2;
        
        // 3. Compensar el +4 que hará process_instruction()
        offset -= 4;
        
        printf("offset calculated: 0x%016lX\n", offset);
        
        // 4. Actualizar PC
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        printf("BEQ taken: PC = 0x%016lX + 0x%016lX = 0x%016lX\n", 
               CURRENT_STATE.PC, offset, NEXT_STATE.PC);
    } else {
        printf("BEQ not taken (Z=0)\n");
    }

}

void execute_b_ne(uint32_t instruction) {
    if (!CURRENT_STATE.FLAG_Z) { 
        execute_b_target(instruction);
    }
}

void execute_b_gt(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_Z==0 && CURRENT_STATE.FLAG_N==0) { 
        execute_b_target(instruction);
    }
}

void execute_b_lt(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_N) { 
        execute_b_target(instruction);
    }
}

void execute_b_ge(uint32_t instruction) {
    if (CURRENT_STATE.FLAG_N==0) { 
        execute_b_target(instruction);
    }
}

void execute_b_le(uint32_t instruction) {
    if ( ! (CURRENT_STATE.FLAG_Z==0 && CURRENT_STATE.FLAG_N==0)) { 
        execute_b_target(instruction);
    }
}




