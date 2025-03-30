
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

