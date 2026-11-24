#include "isa.h"
#include "disasm.h"
#include <stdint.h>
#include <stdio.h>

/* ── 1. 最小程序：只有 HALT ────────────── 验证空循环也能退出 */
uint8_t g_halt[] = { OP_HALT };

/* ── 2. 你那个：1 + 2 ─────────────────── 验证 PUSH 跳 5、ADD/HALT 跳 1 */
uint8_t g_add[] = {
    OP_PUSH, 1,0,0,0,
    OP_PUSH, 2,0,0,0,
    OP_ADD,
    OP_HALT,
};

/* ── 3. 1 + 2 * 3 ──────────────────────── 验证后序：MUL 必须在 ADD 前 */
uint8_t g_arith[] = {
    OP_PUSH, 1,0,0,0,
    OP_PUSH, 2,0,0,0,
    OP_PUSH, 3,0,0,0,
    OP_MUL,
    OP_ADD,
    OP_PRINT,
    OP_HALT,
};

/* ── 4. 大立即数 + 连续 PUSH ──────────── 验证小端拼字节没错位 */
uint8_t g_imm[] = {
    OP_PUSH, 0x78,0x56,0x34,0x12,   /* = 0x12345678 */
    OP_PUSH, 0xFF,0xFF,0xFF,0xFF,   /* = -1 (int32) */
    OP_ADD,
    OP_PRINT,
    OP_HALT,
};

/* ── 5. 操作数截断：最后那个 PUSH 只给了 2 字节 ── 验证 truncated 分支 */
uint8_t g_trunc[] = {
    OP_PUSH, 1,0,0,0,
    OP_PUSH, 2,0,          /* ← 缺 2 字节，故意的 */
};

/* ── 6. 坏 opcode：塞个没定义的 0xAB ──── 验证 ??? 分支 */
uint8_t g_bad[] = {
    OP_PUSH, 1,0,0,0,
    0xAB,
    OP_HALT,
};

/* ── 7. HALT 后面跟垃圾 ──────────────── 验证遇到 HALT 就收工，不越界解 */
uint8_t g_halt_then_junk[] = {
    OP_PUSH, 1,0,0,0,
    OP_HALT,
    0xDE, 0xAD, 0xBE, 0xEF,   /* HALT 之后不该被打印 */
};

typedef struct { const char *name; const uint8_t *code; size_t n; } Case;

#define C(NAME, ARR) { NAME, ARR, sizeof(ARR) }

int main(void){
    Case cases[] = {
        C("halt",          g_halt),
        C("add",           g_add),
        C("arith 1+2 * 3",    g_arith),
        C("imm/endian",    g_imm),
        C("truncated",     g_trunc),
        C("bad opcode",    g_bad),
        C("halt+junk",     g_halt_then_junk),
    };
    int n = sizeof(cases) / sizeof(cases[0]);

    for (int i = 0; i < n; i++) {
        printf("=== [%d] %s (%zu bytes) ===\n", i, cases[i].name, cases[i].n);
        disasm(cases[i].code, cases[i].n, stdout);
        printf("\n");
    }
    return 0;
}
