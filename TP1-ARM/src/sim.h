
#include <stdint.h>

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
void execute_l_sx(uint32_t instruction);
void execute_stur(uint32_t instruction);
void execute_stur_b(uint32_t instruction);
void execute_stur_h(uint32_t instruction);
void execute_ldur(uint32_t instruction);
void execute_ldur_h(uint32_t instruction);
void execute_ldur_b(uint32_t instruction);
void execute_movz(uint32_t instruction);
void execute_add_inmediate(uint32_t instruction);
void execute_add_extended_register(uint32_t instruction);
void execute_mul(uint32_t instruction);
void execute_cbz(uint32_t instruction);
void execute_cbnz(uint32_t instruction);
