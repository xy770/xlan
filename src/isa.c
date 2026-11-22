/*
 * isa.c
 * author: xy770
 * date:   2026-11-22
 */

#include "isa.h"

const char *OP_NAMES[256] = {
    [OP_PUSH]  = "OP_PUSH",
    [OP_ADD]   = "OP_ADD",
    [OP_MUL]   = "OP_MUL",
    [OP_PRINT] = "OP_PRINT",
    [OP_HALT]  = "OP_HALT",
};

uint32_t operand_width(OpCode op){
    switch (op) {
        case OP_PUSH:{
            return 4;
        }
        default:
            return 0;
    }
}
