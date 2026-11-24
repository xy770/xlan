#include <stdio.h>
#include "chunk.h"

int main(void) {
    Chunk c = chunk_new();
    emit_u8(&c, 1);
    emit_u16(&c, 1);
    emit_u32(&c, 1);

    return 0;
}
