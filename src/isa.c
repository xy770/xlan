/*
 * isa.c
 * author: xy770
 * date:   2026-11-22
 */

#include "isa.h"

const char *OP_NAMES[256] = {
    [OP_PUSH]  = "PUSH",
    [OP_ADD]   = "ADD",
    [OP_MUL]   = "MUL",
    [OP_PRINT] = "PRINT",
    [OP_HALT]  = "HALT",
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
