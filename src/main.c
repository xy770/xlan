#include "disasm.h"
#include "isa.h"
#include "vm.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(void){
    //uint8_t code[] = {
    //    OP_PUSH,
    //    0x00,0x00,0x00,0x00,
    //    OP_PUSH,
    //    0x00,0x00,0x01,0x00,
    //    OP_SWAP,
    //    OP_SUB,
    //    OP_PRINT,
    //    OP_HALT,
    //};
    uint8_t code[] = {
        OP_PUSH, 0x07,0,0,0,      /* 0000  n = 7            */

        /* LOOP @0005 : 不变量 = 栈上只有 [n] */
        OP_DUP,                   /* 0005  [n,n]            */
        OP_PRINT,                 /* 0006  打印 n, 弹一份 -> [n] */
        OP_DUP,                   /* 0007  [n,n]            */
        OP_PUSH, 0x01,0,0,0,      /* 0008  [n,n,1]          */
        OP_EQ,                    /* 000D  [n, n==1]        */
        OP_JNZ,  0x3B,0,0,0,      /* 000E  n==1 -> DONE(弹条件) [n] */

        OP_DUP,                   /* 0013  [n,n]            */
        OP_PUSH, 0x02,0,0,0,      /* 0014  [n,n,2]          */
        OP_MOD,                   /* 0019  [n, n%2]         */
        OP_JZ,   0x30,0,0,0,      /* 001A  偶数 -> EVEN(弹n%2) [n] */

        /* --- ODD: n -> 3n+1 --- */
        OP_PUSH, 0x03,0,0,0,      /* 001F  [n,3]            */
        OP_MUL,                   /* 0024  [3n]              */
        OP_PUSH, 0x01,0,0,0,      /* 0025  [3n,1]            */
        OP_ADD,                   /* 002A  [3n+1]            */
        OP_JMP,  0x05,0,0,0,      /* 002B  回 LOOP          */

        /* --- EVEN @0030: n -> n/2 --- */
        OP_PUSH, 0x02,0,0,0,      /* 0030  [n,2]            */
        OP_DIV,                   /* 0035  [n/2]             */
        OP_JMP,  0x05,0,0,0,      /* 0036  回 LOOP          */

        /* DONE @003B */
        OP_POP,                   /* 003B  扔掉最后的 1      */
        OP_HALT,                  /* 003C */
    };
    size_t len = sizeof(code) / sizeof(code[0]);
    disasm(code, len, stdout);

    VM *vm = vm_new(code, len, stdout, NULL);
    vm_run(vm);
    vm_free(&vm);
}
