#include "Utils.h"
#include <stdlib.h>
#include <stdio.h>

uint16_t U16_AT(const uint8_t *ptr) {
    return ptr[0] << 8 | ptr[1];
}

uint16_t U16LE_AT(const uint8_t *ptr) {
    return ptr[1] << 8 | ptr[0];
}

uint32_t U32_AT(const uint8_t *ptr) {
    return ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
}

uint32_t U32LE_AT(const uint8_t *ptr) {
    return ptr[3] << 24 | ptr[2] << 16 | ptr[1] << 8 | ptr[0];
}

uint64_t U64_AT(const uint8_t *ptr) {
    return ((uint64_t)U32_AT(ptr)) << 32 | U32_AT(ptr + 4);
}

uint64_t U64LE_AT(const uint8_t *ptr) {
    return ((uint64_t)U32LE_AT(ptr + 4)) << 32 | U32LE_AT(ptr);
}

const char *MakeFourCCString(uint32_t x) {
    char *str = (char *)malloc(5);
    str[0] = x >> 24;
    str[1] = (x >> 16) & 0xff;
    str[2] = (x >> 8) & 0xff;
    str[3] = x & 0xff;
    str[4] = '\0';

    return str;
}

uint32_t absDiff(uint32_t seq1, uint32_t seq2) {
    return seq1 > seq2 ? seq1 - seq2 : seq2 - seq1;
}

char *itos(int num, char *str) {
    sprintf(str, "%d", num);
    return str;
}

char *dtos(double num, char *str) {
    sprintf(str, "%lf", num);
    return str;
}
