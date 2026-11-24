/*
 * isa.h
 * author: xy770
 * date:   2026-11-22
 */

#ifndef ISA_H
#define ISA_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    OP_PUSH = 0x01, OP_POP = 0x02, OP_DUP = 0x03, OP_SWAP = 0x04,
    OP_ADD = 0x10, OP_SUB = 0x11, OP_MUL = 0x12, OP_DIV = 0x13,
    OP_MOD = 0x14, OP_NEG = 0x15,
    OP_PRINT = 0xF0, OP_HALT = 0xFF,
}OpCode;

extern const char *OP_NAMES[256];

uint32_t operand_width(OpCode op);

#ifdef __cplusplus
}
#endif

#endif
