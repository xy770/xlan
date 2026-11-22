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
    OP_PUSH = 0x01,
    OP_ADD = 0x10, OP_MUL = 0x12,
    OP_PRINT = 0xF0, OP_HALT = 0xFF,
}OpCode;

extern const char *OP_NAMES[256];

uint32_t operand_width(OpCode op);

#ifdef __cplusplus
}
#endif

#endif
