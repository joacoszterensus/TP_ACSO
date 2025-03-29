
#include <stdio.h>
#include <stdint.h>
#include "shell.h"
#include "sim.h"



typedef void (*InstructionFunction)(uint32_t instruction);


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

void process_instruction() {
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    for (int i = 0; instruction_set[i].execute != NULL; i++) {
        if ((instruction & instruction_set[i].mask) == instruction_set[i].opcode) {
            instruction_set[i].execute(instruction);
            break;
        }
    }
}
