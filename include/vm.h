/*
 * vm.h
 * author: xy770
 * date:   2026-11-24
 */

#ifndef VM_H
#define VM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

#define VM_STACK_SIZE 4096

typedef struct{
    uint8_t *code;
    size_t code_size;
    size_t ip;
    uint32_t stack[VM_STACK_SIZE];
    size_t sp;
    FILE *out;
}VM;

VM* vm_new(uint8_t *code, size_t code_size, FILE *out);
void vm_run(VM *vm);
void vm_free(VM **vm);

#ifdef __cplusplus
}
#endif

#endif
