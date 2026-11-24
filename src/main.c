#include "disasm.h"
#include "isa.h"
#include "vm.h"
#include <stddef.h>
#include <stdint.h>

int main(void){
    uint8_t code[] = {
        OP_PUSH,
        0x00,0x01,0x00,0x00,
        OP_PUSH,
        0x00,0x00,0x00,0x00,
        OP_ADD,
        OP_PRINT,
        OP_HALT,
    };

    size_t len = sizeof(code) / sizeof(code[0]);
    disasm(code, len, stdout);

    VM *vm = vm_new(code, len, stdout);
    vm_run(vm);
    vm_free(&vm);
}
