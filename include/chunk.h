/*
 * chunk.h
 * author: xy770
 * date:   2026-11-22
 */

#ifndef CHUNK_H
#define CHUNK_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    size_t cap;
    size_t count;
    uint8_t *code;
}Chunk;

bool chunk_is_empty(Chunk *chunk);
Chunk chunk_new(void);
void chunk_free(Chunk *chunk);
void emit_u8(Chunk *chunk, uint8_t v);
void emit_u16(Chunk *chunk, uint16_t v);
void emit_u32(Chunk *chunk, uint32_t v);

#ifdef __cplusplus
}
#endif

#endif
