/*
 * chunk.c
 * author: xy770
 * date:   2026-11-22
 */

#include "chunk.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int chunk_reserve(Chunk *chunk, size_t size){
    if(chunk == NULL) return 1;

    if(chunk->cap >= size + chunk->count) return 0;
    size = (size + chunk->count + 1) * 2;

    uint8_t *tmp = realloc(chunk->code, size);
    if(tmp == NULL) return 1;
    chunk->code = tmp;

    chunk->cap = size;
    return 0;
}

Chunk chunk_new(void){
    Chunk chunk = { 0 };

    chunk.code = malloc(sizeof(chunk.code[0]) * 1024);
    if(chunk.code == NULL) return chunk;
    
    chunk.cap = 1024;
    chunk.count = 0;

    return chunk;
}

void chunk_free(Chunk *chunk){
    if(chunk == NULL) return;

    if(chunk->code) free(chunk->code);
    chunk->code = NULL;

    chunk->cap = 0;
    chunk->count = 0;
}

void emit_u8(Chunk *chunk, uint8_t v){
    if(chunk_reserve(chunk, 1)) return;

    chunk->code[(chunk->count)++] = v;
}

void emit_u16(Chunk *chunk, uint16_t v){
    if(chunk_reserve(chunk, 2)) return;

    chunk->code[(chunk->count)++] = (uint8_t)(v & 0xFF);
    chunk->code[(chunk->count)++] = (uint8_t)((v >> 8) & 0xFF);
}

void emit_u32(Chunk *chunk, uint32_t v){
    if(chunk_reserve(chunk, 4)) return;

    chunk->code[(chunk->count)++] = (uint8_t)(v & 0xFF);
    chunk->code[(chunk->count)++] = (uint8_t)((v >> 8) & 0xFF);
    chunk->code[(chunk->count)++] = (uint8_t)((v >> 16) & 0xFF);
    chunk->code[(chunk->count)++] = (uint8_t)((v >> 24) & 0xFF);
}
