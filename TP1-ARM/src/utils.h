#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>


int64_t signextend64(uint32_t value, int bits);
int setFlagN(uint64_t result);
int setFlagZ(uint64_t result);


#endif // UTILS_H
