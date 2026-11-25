/*
 * disasm.c
 * author: xy770
 * date:   2026-11-24
 */

#include "disasm.h"
#include "isa.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void print_err(const char *message, FILE *out){
    fprintf(out, "[ERROR]: %s\n", message);
}

void print_op(size_t address, OpCode op_code, uint8_t *operand, FILE *out){
    size_t width = operand_width(op_code);
    fprintf(out, "%04zX %s", address, OP_NAMES[op_code]);
    if(operand != NULL && width > 0){
        for (size_t i = 0; i < width; ++i) {
            fprintf(out, " %02X", operand[i]);
        }
    }
    fprintf(out, "\n");
}

static inline int8_t is_width_enough(uint32_t width, size_t i, size_t len, FILE *out){
    if(i + width >= len){
        print_err("there is no enough byte for PUSH", out);
        return 0;
    }

    return 1;
}

void disasm(uint8_t *code, size_t len, FILE *out){
    for (size_t i = 0; i < len; ++i) {
        switch (code[i]) {
            case OP_PUSH:{
                uint32_t width = operand_width(OP_PUSH);
                if(!is_width_enough(width, i, len, out)) return;
                print_op(i, OP_PUSH, code + i + 1, out);
                i += width;
                break;
            }
            case OP_DUP:{
                print_op(i, OP_DUP, NULL, out);
                break;
            }
            case OP_SWAP:{
                print_op(i, OP_SWAP, NULL, out);
                break;
            }
            case OP_POP:{
                print_op(i, OP_POP, NULL, out);
                break;
            }
            case OP_ADD:{
                print_op(i, OP_ADD, NULL, out);
                break;
            }
            case OP_SUB:{
                print_op(i, OP_SUB, NULL, out);
                break;
            }
            case OP_MUL:{
                print_op(i, OP_MUL, NULL, out);
                break;
            }
            case OP_DIV:{
                print_op(i, OP_DIV, NULL, out);
                break;
            }
            case OP_MOD:{
                print_op(i, OP_MOD, NULL, out);
                break;
            }
            case OP_NEG:{
                print_op(i, OP_NEG, NULL, out);
                break;
            }
            case OP_EQ:{
                print_op(i, OP_EQ, NULL, out);
                break;
            }
            case OP_LT:{
                print_op(i, OP_LT, NULL, out);
                break;
            }
            case OP_LE:{
                print_op(i, OP_LE, NULL, out);
                break;
            }
            case OP_GT:{
                print_op(i, OP_GT, NULL, out);
                break;
            }
            case OP_GE:{
                print_op(i, OP_GE, NULL, out);
                break;
            }
            case OP_NOT:{
                print_op(i, OP_NOT, NULL, out);
                break;
            }
            case OP_JMP:{
                uint32_t width = operand_width(OP_JMP);
                if(!is_width_enough(width, i, len, out)) return;
                print_op(i, OP_JMP, code + i + 1, out);
                i += width;
                break;
            }
            case OP_JZ:{
                uint32_t width = operand_width(OP_JZ);
                if(!is_width_enough(width, i, len, out)) return;
                print_op(i, OP_JZ, code + i + 1, out);
                i += width;
                break;
            }
            case OP_JNZ:{
                uint32_t width = operand_width(OP_JNZ);
                if(!is_width_enough(width, i, len, out)) return;
                print_op(i, OP_JNZ, code + i + 1, out);
                i += width;
                break;
            }
            case OP_PRINT:{
                print_op(i, OP_PRINT, NULL, out);
                break;
            }
            case OP_HALT:{
                print_op(i, OP_HALT, NULL, out);
                return;
            }
            default:{
                char err_message[256] = { 0 };
                snprintf(err_message, sizeof(err_message),  \
                        "Unknown byte code %#02X at %02zX",    \
                        code[i], i);
                print_err(err_message, out);
                return;
            }
        }
    }
}
