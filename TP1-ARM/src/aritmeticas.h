#ifndef ARITMETICAS_H
#define ARITMETICAS_H

#include <stdint.h>

void execute_adds_inmediate(uint32_t instruction);
void execute_adds_extended_register(uint32_t instruction);
void execute_sub_inmediate(uint32_t instruction);
void execute_sub_extended_register(uint32_t instruction);
void execute_add_inmediate(uint32_t instruction);
void execute_add_extended_register(uint32_t instruction);
void execute_mul(uint32_t instruction);

#endif // ARITMETICAS_H