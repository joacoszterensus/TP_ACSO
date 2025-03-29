
#include <stdint.h>

#ifndef SIM_H
#define SIM_H

typedef void (*InstructionFunction)(uint32_t instruction);


typedef struct {
    uint32_t opcode;
    uint32_t mask;
    InstructionFunction execute;
} InstructionEntry;


#include "aritmeticas.h"
#include "logicas.h"
#include "branch.h"
#include "memoria.h"
#include "mov.h"

#endif 


void execute_l_sx(uint32_t instruction);
void execute_stur(uint32_t instruction);
void execute_stur_b(uint32_t instruction);
void execute_stur_h(uint32_t instruction);
void execute_ldur(uint32_t instruction);
void execute_ldur_h(uint32_t instruction);
void execute_ldur_b(uint32_t instruction);
void execute_movz(uint32_t instruction);
