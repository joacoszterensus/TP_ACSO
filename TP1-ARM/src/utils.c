#include <stdint.h>


int64_t signextend64(uint32_t value, int bits) {
    int shift = 64 - bits;
    return ((int64_t)value << shift) >> shift;
}

int setFlagN(uint64_t result) {
    return (result >> 63) & 1;
}
int setFlagZ(uint64_t result) {
    return (result == 0);
}