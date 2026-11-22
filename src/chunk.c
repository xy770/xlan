/*
 * chunk.c
 * author: xy770
 * date:   2026-11-22
 */

#include "chunk.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void chunk_with(Chunk *chunk, size_t size){
    if(chunk == NULL) return;

    uint8_t *tmp = realloc(chunk->code, size);
    if(tmp == NULL) return;
    chunk->code = tmp;

    chunk->cap = size;
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

    if(!chunk->code) free(chunk->code);
    chunk->code = NULL;

    chunk->cap = 0;
    chunk->count = 0;
}

void emit_u8(Chunk *chunk, uint8_t v){
    if(chunk->cap - 1 < chunk->count){
        size_t size = (chunk->cap + 1) * 2;
        chunk_with(chunk, size);
        
        if(chunk->cap - 1 < chunk->count) return;
    }

    chunk->code[(chunk->count)++] = v;
}

void emit_u16(Chunk *chunk, uint16_t v){
    if(chunk->cap - 2 < chunk->count){
        size_t size = (chunk->cap + 2) * 2;
        chunk_with(chunk, size);
        
        if(chunk->cap - 2 < chunk->count) return;
    }

    chunk->count += 2;
}

void emit_u32(Chunk *chunk, uint32_t v){
    if(chunk->cap - 4 < chunk->count){
        size_t size = (chunk->cap + 4) * 2;
        chunk_with(chunk, size);
        
        if(chunk->cap - 4 < chunk->count) return;
    }

}
