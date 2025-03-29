#ifndef LOGICAS_H
#define LOGICAS_H

#include <stdint.h>
#include "shell.h"

void execute_HLT(uint32_t instruction);
void execute_CMP_inmediate(uint32_t instruction);
void execute_CMP_extended_register(uint32_t instruction);
void execute_ands_shifted_register(uint32_t instruction);
void execute_eor_shifted_register(uint32_t instruction);
void execute_orr_shifted_register(uint32_t instruction);

#endif // LOGICAS_H
