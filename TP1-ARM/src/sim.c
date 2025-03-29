
#include <stdio.h>
#include <stdint.h>
#include "shell.h"
#include "sim.h"



typedef void (*InstructionFunction)(uint32_t instruction);

typedef struct {
    uint32_t opcode;
    uint32_t mask;
    InstructionFunction execute;
} InstructionEntry;

InstructionEntry instruction_set[] = {
    {0xB1     << 24, 0xFF000000, execute_adds_inmediate},        
    {0xAB0    << 20, 0xFFF00000, execute_adds_extended_register},
    {0xF1     << 24, 0xFF000000, execute_sub_inmediate},         
    {0xEB0    << 20, 0xFFF00000, execute_sub_extended_register}, 
    {0xD44    << 20, 0xFFF00000, execute_HLT},                   
    {0xEB0    << 20, 0xFFF00000, execute_CMP_extended_register}, 
    {0xF0     << 24, 0xFF000000, execute_CMP_inmediate},         
    {0xEA     << 24, 0xFF000000, execute_ands_shifted_register}, 
    {0xCA     << 24, 0xFF000000, execute_eor_shifted_register},  
    {0xAA     << 24, 0xFF000000, execute_orr_shifted_register},  
    {0x03     << 26, 0xFC000000, execute_b_target},              
    {0x3587C0 << 10, 0xFFFFFC00, execute_br},   
    {0x54     << 24, 0xFF000000, execute_b_conditional},  
    {0xD3     << 24, 0xFF000000, execute_l_sx},
    {0xF80    << 20, 0xFFF00000, execute_stur},
    {0x380    << 20, 0xFFF00000, execute_stur_b},
    {0x780    << 20, 0xFFF00000, execute_stur_h},
    {0xF84    << 20, 0xFFF00000, execute_ldur},
    {0x784    << 20, 0xFFF00000, execute_ldur_h},
    {0x384    << 20, 0xFFF00000, execute_ldur_b},
    {0xD2     << 24, 0xFF000000, execute_movz},
    {0x91     << 24, 0xFF000000, execute_add_inmediate},        
    {0x8B0    << 20, 0xFFF00000, execute_add_extended_register},
    {0x9B0    << 20, 0xFFF00000, execute_mul},
    {0xB4     << 24, 0xFF000000, execute_cbz},
    {0xB5     << 24, 0xFF000000, execute_cbnz},                    
    
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
    for (int i = 0; instruction_set[i].execute != NULL; i++) {
        if ((instruction & instruction_set[i].mask) == instruction_set[i].opcode) {
            instruction_set[i].execute(instruction);
            break;
        }
    }
}

int64_t signextend64(uint32_t value, int bits) {
    int shift = 64 - bits;
    return ((int64_t)value << shift) >> shift;
}

void execute_adds_inmediate(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    uint32_t imm12 = (instruction >> 10) & 0xFFF; 
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        
    NEXT_STATE.FLAG_Z = (result == 0);            
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_adds_extended_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    uint32_t rm = (instruction >> 16) & 0x1F;  
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        
    NEXT_STATE.FLAG_Z = (result == 0);       
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_sub_inmediate(uint32_t instruction) {
    uint32_t rd    = (instruction >> 0) & 0x1F;     
    uint32_t rn    = (instruction >> 5) & 0x1F;      
    uint32_t imm12 = (instruction >> 10) & 0xFFF;     
    uint32_t shift = (instruction >> 22) & 0x3;       

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    operand2 = ~operand2;  
    int64_t result = CURRENT_STATE.REGS[rn] + operand2 + 1;  
    
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        
    NEXT_STATE.FLAG_Z = (result == 0);            

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

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        
    NEXT_STATE.FLAG_Z = (result == 0);            

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_HLT(uint32_t instruction) {

    RUN_BIT = 0;
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return;
}

void execute_CMP_inmediate(uint32_t instruction) {
    uint32_t rn = (instruction >> 5) & 0x1F;     
    uint32_t imm12 = (instruction >> 10) & 0xFFF; 
    uint32_t shift = (instruction >> 22) & 0x3;  

    uint64_t operand2 = imm12;

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;

    NEXT_STATE.FLAG_N = (result >> 63) & 1; 
    NEXT_STATE.FLAG_Z = (result == 0);      
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_CMP_extended_register(uint32_t instruction) {
    uint32_t rn = (instruction >> 5) & 0x1F;   
    uint32_t rm = (instruction >> 16) & 0x1F; 
    uint32_t shift = (instruction >> 22) & 0x3; 

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    if (shift == 0b01) {
        operand2 <<= 12;  
    } 

    uint64_t result = CURRENT_STATE.REGS[rn] - operand2;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        
    NEXT_STATE.FLAG_Z = (result == 0);             
    
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_ands_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;  
    uint32_t rm = (instruction >> 16) & 0x1F;  

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    

    uint64_t result = CURRENT_STATE.REGS[rn] & operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;        
    NEXT_STATE.FLAG_Z = (result == 0);             

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_eor_shifted_register(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   // Bits 0-4: Rd
    uint32_t rn = (instruction >> 5) & 0x1F;   // Bits 5-9: Rn
    uint32_t rm = (instruction >> 16) & 0x1F;  // Bits 16-20: Rm

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] ^ operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

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
    int64_t imm26 = instruction & 0x03FFFFFF;
    int64_t offset = signextend64(imm26, 26) << 2;
    
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}

void execute_br(uint32_t instruction) {
    uint32_t rn = (instruction >> 5) & 0x1F;  
    NEXT_STATE.PC = CURRENT_STATE.REGS[rn];   
}

void execute_b_conditional(uint32_t instruction) {
    uint32_t cond = (instruction) & 0xF; 
    
    for (int i = 0; b_cond_set[i].execute != NULL; i++) {
        if ((cond & b_cond_set[i].mask) == b_cond_set[i].opcode) {
            b_cond_set[i].execute(instruction);
            break;
        }
    }
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

void execute_l_sx(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    uint32_t imms = (instruction >> 10) & 0x3F;  
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

void execute_movz(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;    
    uint32_t imm16 = (instruction >> 5) & 0xFFFF; 
    uint32_t hw = (instruction >> 21) & 0x3;   
    
    if (hw == 0) {
        NEXT_STATE.REGS[rd] = imm16;  
    }

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_add_inmediate(uint32_t instruction) {
    uint32_t rd = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
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
    uint32_t rd = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    uint32_t rm = (instruction >> 16) & 0x1F;  
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
    uint32_t rd = (instruction >> 0) & 0x1F;   
    uint32_t rn = (instruction >> 5) & 0x1F;   
    uint32_t rm = (instruction >> 16) & 0x1F;  

    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = CURRENT_STATE.REGS[rn] * operand2;

    NEXT_STATE.REGS[rd] = result;
     
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
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