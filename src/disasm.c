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

void disasm(uint8_t *code, size_t len, FILE *out){
    for (size_t i = 0; i < len; ++i) {
        switch (code[i]) {
            case OP_PUSH:{
                uint32_t width = operand_width(OP_PUSH);
                if(i + width >= len){
                    print_err("there is not enough byte for PUSH", out);
                    return;
                }
                print_op(i, OP_PUSH, code + i + 1, out);
                i += width;
                break;
            }
            case OP_ADD:{
                print_op(i, OP_ADD, NULL, out);
                break;
            }
            case OP_MUL:{
                print_op(i, OP_MUL, NULL, out);
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
