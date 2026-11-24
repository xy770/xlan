/*
 * disasm.h
 * author: xy770
 * date:   2026-11-24
 */

#ifndef DISASM_H
#define DISASM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

void disasm(uint8_t *code, size_t len, FILE *out);

#ifdef __cplusplus
}
#endif

#endif
