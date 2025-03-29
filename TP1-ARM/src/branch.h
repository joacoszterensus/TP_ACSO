#ifndef BRANCH_H
#define BRANCH_H

#include <stdint.h>

void execute_b_target(uint32_t instruction);
void execute_br(uint32_t instruction);
void execute_b_conditional(uint32_t instruction);
void execute_b_eq(uint32_t instruction);
void execute_b_ne(uint32_t instruction);
void execute_b_gt(uint32_t instruction);
void execute_b_lt(uint32_t instruction);
void execute_b_ge(uint32_t instruction);
void execute_b_le(uint32_t instruction);
void execute_cbz(uint32_t instruction);
void execute_cbnz(uint32_t instruction);

#endif // BRANCH_H
