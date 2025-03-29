#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

void execute_stur(uint32_t instruction);
void execute_stur_b(uint32_t instruction);
void execute_stur_h(uint32_t instruction);
void execute_ldur(uint32_t instruction);
void execute_ldur_h(uint32_t instruction);
void execute_ldur_b(uint32_t instruction);

#endif // MEMORIA_H
