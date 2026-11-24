#include "../include/isa.h"
#include "../include/disasm.h"
#include <stdint.h>

uint8_t code_add[] = {
    OP_PUSH,
    1,0,0,0,
    OP_PUSH,
    2,0,0,0,
    OP_ADD,
    OP_HALT,
};

int main(void){
    disasm(code_add, sizeof(code_add) / sizeof(code_add[0]), stdout);

    return 0;
}
