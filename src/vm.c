/*
 * vm.c
 * author: xy770
 * date:   2026-11-24
 */

#include "vm.h"
#include "isa.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline void push(VM *vm, uint32_t v){
    assert(vm->sp + 1 <= VM_STACK_SIZE);

    vm->stack[(vm->sp)++] = v;
}

static inline uint32_t pop(VM *vm){
    return vm->stack[--(vm->sp)];
}

static inline uint32_t load_u32(VM *vm){
    uint32_t result = 0;
    result += vm->code[(vm->ip)++];
    result += vm->code[(vm->ip)++] << 8;
    result += vm->code[(vm->ip)++] << 16;
    result += vm->code[(vm->ip)++] << 24;
    return result;
}

VM* vm_new(uint8_t *code, size_t code_size, FILE *out){
    if(code == NULL) return NULL;

    VM *vm = malloc(sizeof(*vm));
    if(vm == NULL) return NULL;
    
    vm->ip = 0;
    vm->sp = 0;
    vm->code_size = code_size;
    vm->out = out;
    vm->code = code;
    return vm;
}

void vm_run(VM *vm){
    if(vm == NULL) return;

    for (; vm->ip < vm->code_size; ) {
        switch (vm->code[vm->ip]) {
            case OP_PUSH:{
                ++(vm->ip);
                uint32_t v = load_u32(vm);
                push(vm, v);
                break;
            }
            case OP_ADD:{
                ++(vm->ip);
                uint32_t a = pop(vm);
                uint32_t b = pop(vm);
                push(vm, b + a);
                break;
            }
            case OP_MUL:{
                ++(vm->ip);
                uint32_t a = pop(vm);
                uint32_t b = pop(vm);
                push(vm, b * a);
                break;
            }
            case OP_PRINT:{
                ++(vm->ip);
                uint32_t num = pop(vm);
                fprintf(vm->out, "%u", num);
                break;
            }
            case OP_HALT:{
                goto halt;
            }
            default:{
                // TODO!
                goto halt;
            }
        }
    }

halt:
    return;
}

void vm_free(VM **vm){
    if(vm == NULL || *vm == NULL) return;

    free(*vm);
    *vm = NULL;
}
