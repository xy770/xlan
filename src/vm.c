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

static inline int32_t pop(VM *vm){
    return vm->stack[--(vm->sp)];
}

static inline void dup(VM *vm){
    assert(vm->sp >= 1);
    push(vm, vm->stack[vm->sp-1]);
}

static inline void swap(VM *vm){
    assert(vm->sp >= 2);
    int32_t a = pop(vm);
    int32_t b = pop(vm);
    push(vm, a);
    push(vm, b);
}

static inline uint32_t load_u32(VM *vm){
    uint32_t result = 0;
    result += vm->code[(vm->ip)++];
    result += vm->code[(vm->ip)++] << 8;
    result += vm->code[(vm->ip)++] << 16;
    result += vm->code[(vm->ip)++] << 24;
    return result;
}

void vm_dump(void){
    // TODO!
}

VM* vm_new(uint8_t *code, size_t code_size, FILE *out, FILE *dump_out){
    if(code == NULL) return NULL;

    VM *vm = malloc(sizeof(*vm));
    if(vm == NULL) return NULL;
    
    vm->ip = 0;
    vm->sp = 0;
    vm->code_size = code_size;
    vm->out = out;
    vm->code = code;
    vm->dump_out = dump_out;
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
            case OP_POP:{
                ++(vm->ip);
                pop(vm);
                break;
            }
            case OP_DUP:{
                ++(vm->ip);
                dup(vm);
                break;
            }
            case OP_SWAP:{
                ++(vm->ip);
                swap(vm);
                break;
            }
            case OP_ADD:{
                ++(vm->ip);
                int32_t a = pop(vm);
                int32_t b = pop(vm);
                push(vm, b + a);
                break;
            }
            case OP_SUB:{
                ++(vm->ip);
                int32_t a = pop(vm);
                int32_t b = pop(vm);
                push(vm, b - a);
                break;
            }
            case OP_MUL:{
                ++(vm->ip);
                int32_t a = pop(vm);
                int32_t b = pop(vm);
                push(vm, b * a);
                break;
            }
            case OP_DIV:{
                ++(vm->ip);
                int32_t a = pop(vm);
                int32_t b = pop(vm);
                if(a == 0) goto halt;
                push(vm, b / a);
                break;
            }
            case OP_MOD:{
                ++(vm->ip);
                int32_t a = pop(vm);
                int32_t b = pop(vm);
                if(a == 0) goto halt;
                push(vm, b % a);
                break;
            }
            case OP_NEG:{
                ++(vm->ip);
                int32_t a = pop(vm);
                push(vm, -a);
                break;
            }
            case OP_EQ:{
                ++(vm->ip);
                int32_t lv = pop(vm);
                int32_t rv = pop(vm);
                push(vm, lv == rv);
                break;
            }
            case OP_LT:{
                ++(vm->ip);
                int32_t lv = pop(vm);
                int32_t rv = pop(vm);
                push(vm, lv < rv);
                break;
            }
            case OP_LE:{
                ++(vm->ip);
                int32_t lv = pop(vm);
                int32_t rv = pop(vm);
                push(vm, lv <= rv);
                break;
            }
            case OP_GT:{
                ++(vm->ip);
                int32_t lv = pop(vm);
                int32_t rv = pop(vm);
                push(vm, lv > rv);
                break;
            }
            case OP_GE:{
                ++(vm->ip);
                int32_t lv = pop(vm);
                int32_t rv = pop(vm);
                push(vm, lv >= rv);
                break;
            }
            case OP_NOT:{
                ++(vm->ip);
                int32_t v = pop(vm);
                push(vm, !v);
                break;
            }
            case OP_JMP:{
                ++(vm->ip);
                uint32_t addr = load_u32(vm);
                vm->ip = addr;
                break;
            }
            case OP_JZ:{
                ++(vm->ip);
                uint32_t addr = load_u32(vm);
                if(pop(vm) == 0) vm->ip = addr;
                break;
            }
            case OP_JNZ:{
                ++(vm->ip);
                uint32_t addr = load_u32(vm);
                if(pop(vm) != 0) vm->ip = addr;
                break;
            }
            case OP_PRINT:{
                ++(vm->ip);
                int32_t num = pop(vm);
                fprintf(vm->out, "%d", num);
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
