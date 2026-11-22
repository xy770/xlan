# ToyVM ISA v1（教学用栈式虚拟机指令集）

> 这份表是**合同**。编码器（汇编器/编译器后端）、VM 主循环、反汇编器三份代码必须严格遵守同一张表。
> **改一处 = 三处同改**。想加新指令只在对应段尾**追加编号**，绝不插队重排。

---

## 1. 机器模型

| 项 | 约定 |
|---|---|
| 字长 | 32 位有符号整数（`int32_t`） |
| 字节序 | **小端**（低字节在前） |
| 操作数栈 | `int32_t stack[4096]`，`sp` 指向栈顶下一格 |
| 全局内存 | `int32_t globals[65536]`，同时承载全局变量与数组区 |
| 调用栈 | 与操作数栈**同一块** `stack`，用 `bp` 划分栈帧 |
| 指令指针 | `size_t ip`，指向**下一条待读字节** |
| 操作码宽度 | 固定 1 字节 |
| 编码格式 | ` opcode ` + 操作数（0–5 字节变长） |

### 栈效果记号

```
箭头左边 = 执行前栈顶布局     箭头右边 = 执行后栈顶布局
                栈顶永远在【右边】
```

| 写法 | 含义 |
|---|---|
| `→ v` | 凭空压一个 v |
| `a →` | 弹掉一个 |
| `a → a a` | 复制栈顶 |
| `b a → a b` | 交换顶俩 |
| `b a → a+b` | 顶俩合成一个（**先弹的是 b = 右操作数**） |
| `idx base → v` | 弹俩算地址，压回读到的值 |

> ⚠️ `SUB` / `DIV` / `MOD` 顺序敏感：`PUSH 10; PUSH 3; SUB` ⇒ `10-3=7`（不是 -7）。

---

## 2. 操作数宽度速查

| 记号 | 字节 | 用途 | 编码器 | VM 读取 |
|---|---|---|---|---|
| — | 0 | 无操作数 | — | — |
| `slot8` | 1 | 局部变量槽号 0–255 | `emit8` | `RD8` |
| `addr16` | 2 | 全局内存地址 0–65535 | `emit16` | `RD16` |
| `imm32` | 4 | 立即数 / 跳转目标 | `emit32` | `RD32` |
| `argc8` | 1 | 参数个数（跟在 CALL 的 addr 后） | `emit8` | `RD8` |

**保留号**：`0x00` 非法（读到即字节码损坏）；`0xFF` 固定为 `HALT`。

---

## 3. 操作码分区总表

```
0x00       保留（非法 opcode 哨兵）
0x01–0x0F  栈与立即数
0x10–0x1F  算术
0x20–0x2F  比较与逻辑
0x30–0x3F  变量与内存
0x40–0x4F  控制流
0x50–0x5F  函数
0x60–0xEF  （预留：浮点 / 字符串 / GC / FFI）
0xF0–0xFD  I/O
0xFE       调试
0xFF       HALT
```

段内**只追加不插队**。反汇编器可用 `op >> 4` 快速分类。

---

## 4. 指令详表

### 4.1 栈与立即数（0x01–0x0F）

| Op | 助记符 | 操作数 | 栈效果 | 语义 |
|----|--------|--------|--------|------|
| `0x01` | `PUSH` | `imm32` | `→ v` | 压立即数 |
| `0x02` | `POP`  | — | `a →` | 丢弃栈顶 |
| `0x03` | `DUP`  | — | `a → a a` | 复制栈顶 |
| `0x04` | `SWAP` | — | `b a → a b` | 交换顶俩 |

### 4.2 算术（0x10–0x1F）

| Op | 助记符 | 栈效果 | 语义 | 陷阱 |
|----|--------|--------|------|------|
| `0x10` | `ADD` | `b a → a+b` | 加 | — |
| `0x11` | `SUB` | `b a → a-b` | 减 | **次顶减栈顶** |
| `0x12` | `MUL` | `b a → a*b` | 乘 | 溢出不检（第一版） |
| `0x13` | `DIV` | `b a → a/b` | 整数除 | **b==0 → trap** |
| `0x14` | `MOD` | `b a → a%b` | 取模 | **b==0 → trap** |
| `0x15` | `NEG` | `a → -a` | 一元负 | — |

### 4.3 比较与逻辑（0x20–0x2F）· 结果 0/1 压栈

| Op | 助记符 | 栈效果 |
|----|--------|--------|
| `0x20` | `EQ`  | `b a → a==b` |
| `0x21` | `NE`  | `b a → a!=b` |
| `0x22` | `LT`  | `b a → a< b` |
| `0x23` | `LE`  | `b a → a<=b` |
| `0x24` | `GT`  | `b a → a> b` |
| `0x25` | `GE`  | `b a → a>=b` |
| `0x26` | `NOT` | `a → !a`（0→1，非0→0） |

> `&&` / `||` **不设指令** —— 编译器用 `JZ`/`JNZ` 短路展开。
> `true`/`false` **不设指令** —— 编译器 emit `PUSH 1` / `PUSH 0`。

### 4.4 变量与内存（0x30–0x3F）

| Op | 助记符 | 操作数 | 栈效果 | 语义 |
|----|--------|--------|--------|------|
| `0x30` | `LOAD`   | `slot8`  | `→ v` | 读局部：`stack[bp + slot]` |
| `0x31` | `STORE`  | `slot8`  | `v →` | 写局部：`stack[bp + slot] = v` |
| `0x32` | `GLOAD`  | `addr16` | `→ v` | 读全局：`globals[addr]` |
| `0x33` | `GSTORE` | `addr16` | `v →` | 写全局：`globals[addr] = v` |
| `0x34` | `LOADI`  | — | `idx base → v` | 读 `globals[base+idx]` |
| `0x35` | `STOREI` | — | `v idx base →` | 写 `globals[base+idx]` |

> **`slot` 的语义**：编译器负责把 `(argc + 2 + 局部变量序号)` 折进 slot 值里，**VM 只看 `bp+slot`，不知道 argc**。这是分层的关键。
>
> **边界检查**：`LOADI/STOREI` 必须 `assert(idx >= 0 && base+idx < GLOBAL_MAX)`。`LOAD/STORE` 建议 `assert(bp+slot < STACK_MAX)`。

### 4.5 控制流（0x40–0x4F）

| Op | 助记符 | 操作数 | 栈效果 | 语义 |
|----|--------|--------|--------|------|
| `0x40` | `JMP`  | `addr32` | — | `ip = addr` |
| `0x41` | `JZ`   | `addr32` | `c →` | `c==0` 则 `ip=addr`，否则 `ip+=4` |
| `0x42` | `JNZ`  | `addr32` | `c →` | `c!=0` 则 `ip=addr`，否则 `ip+=4` |

**语法糖映射（全靠这三条 + 回填 backpatch）**：

```
if (c) A else B   ⇒  <c> JZ L_else  <A> JMP L_end  L_else: <B>  L_end:
while (c) B       ⇒  L_loop: <c> JZ L_end  <B> JMP L_loop  L_end:
for (i;s;b) B     ⇒  <i> L_loop: <s> JZ L_end  <B> <b> JMP L_loop  L_end:
a && b            ⇒  <a> DUP JZ L_end POP <b>  L_end:
a || b            ⇒  <a> DUP JNZ L_end POP <b> L_end:
c ? a : b         ⇒  <c> JZ L_else <a> JMP L_end L_else: <b> L_end:
```

### 4.6 函数（0x50–0x5F）

| Op | 助记符 | 操作数 | 栈效果 |
|----|--------|--------|--------|
| `0x50` | `CALL` | `addr32 argc8` | `arg0..argN-1 → rv` |
| `0x51` | `RET`  | — | `rv →` |

#### CALL 协议（写死，别改）

**调用前**：调用者按 `arg0, arg1, ..., argN-1` 顺序 `PUSH`（arg0 最先入栈，位于最低址）。

**CALL 执行**：

```
1. tgt = RD32(); argc = RD8();
2. PUSH(ip)          ; 返回地址（下一条指令）
3. PUSH(bp)          ; 旧 BP（动态链）
4. bp = sp - argc    ; 新 BP 指向 arg0 的槽
5. ip = tgt
```

**RET 执行**：

```
1. rv = POP()        ; 返回值
2. sp = bp           ; 一把梭弹掉本帧所有局部变量
3. bp = POP()        ; 恢复旧 BP
4. ip = POP()        ; 跳回调用者
5. PUSH(rv)          ; 返回值留给调用者
```

#### 栈帧布局（CALL 完成之后、进入函数体）

```
高地址
   ┌──────────────┐
   │   arg0       │ ← BP            （LOAD slot0 读到这）
   │   arg1       │ ← BP+1
   │   ...        │
   │   argN-1     │ ← BP+argc-1
   ├──────────────┤
   │  返回地址 ip  │ ← BP+argc       （RET 用）
   │  旧 BP       │ ← BP+argc+1     （RET 用）
   ├──────────────┤
   │  局部 slot0  │ ← BP+argc+2     （编译器 emit LOAD 时加上这个偏移）
   │  局部 slot1  │
   │   ...        │
   │  临时求值区  │ ← sp 在此浮动
   └──────────────┘
低地址
```

> 编译器在符号表里为每个函数记录 `argc`，emit `LOAD/STORE` 时统一加 `argc+2`，**VM 侧永远只写 `stack[bp+slot]`**。

### 4.7 I/O 与调试（0xF0–0xFF）

| Op | 助记符 | 栈效果 | 语义 |
|----|--------|--------|------|
| `0xF0` | `PRINT`  | `a →` | `printf("%d\n", a)` |
| `0xF1` | `PRINTI` | `a →` | 同上（预留整数专版，将来可与 PRINTF 区分） |
| `0xF2` | `PRINTC` | `a →` | `putchar(a & 0xFF)` |
| `0xFE` | `BREAK`  | — | 触发调试器 / `abort()`，供单步断点 |
| `0xFF` | `HALT`   | — | 结束 VM 主循环 |

---

## 5. 操作数宽度表（编码器 / VM / 反汇编器三处共用）

| Opcode | 总长 | 说明 |
|--------|------|------|
| `PUSH` `JMP` `JZ` `JNZ` | 5 | op + imm32/addr32 |
| `CALL` | 6 | op + addr32 + argc8 |
| `GLOAD` `GSTORE` | 3 | op + addr16 |
| `LOAD` `STORE` | 2 | op + slot8 |
| 其余（POP/DUP/SWAP/ADD/SUB/MUL/DIV/MOD/NEG/ EQ/NE/LT/LE/GT/GE/NOT/ LOADI/STOREI/RET/PRINT/PRINTI/PRINTC/BREAK/HALT） | 1 | 仅有 op |

**加新指令流程**：① 在段尾分配 op 号 → ② 更新本表 → ③ 更新 `oprand_width()` → ④ 更新 `MNEM[]` → ⑤ 写 VM case → ⑥ 写编码器 helper → ⑦ 加一条 disasm 测试。**七步缺一不可**。

---

## 6. 二进制文件格式（`.toyb`）

```
偏移  大小   字段
─────────────────────────────────
0x00  4B     magic   = "TOYB" (0x42 0x59 0x4F 0x54 小端存)
0x04  1B     version = 0x01
0x05  1B     flags   (bit0: 有调试符号表; 其余保留)
0x06  2B     entry   = 入口地址（通常 = main 的字节码偏移）
0x08  4B     code_size  (字节)
0x0C  4B     data_size  (全局区大小，加载时清零 globals[0..data_size))
0x10  ...    code[code_size]    ← 字节码从这里开始
...   ...    (可选) 符号表段     —— 调试用，格式 v1 不定义
```

**加载流程**：读 header → 校验 magic/version → `fread(code, 1, code_size, f)` → `memset(globals, 0, data_size)` → `ip = entry; run(code);`

> 💡 版本号的意义：将来改 ISA（比如加浮点段）时，旧 `.toyb` 能被识别并拒绝执行，而不是跑出一堆玄学 `bad opcode`。

---

## 7. 手搓黄金测试（Step 1 用，写编译器之前的参照物）

### 7.1 `1 + 2 * 3` ⇒ 输出 `7`

```
地址  字节                     反汇编
─────────────────────────────────────────
0000  01 01 00 00 00          PUSH 1
0005  01 02 00 00 00          PUSH 2
000A  01 03 00 00 00          PUSH 3
000F  12                      MUL
0010  10                      ADD
0011  F0                      PRINT
0012  FF                      HALT
```

C 数组字面量：

```c
uint8_t golden_arith[] = {
    0x01,1,0,0,0,
    0x01,2,0,0,0,
    0x01,3,0,0,0,
    0x12,
    0x10,
    0xF0,
    0xFF,
};
```

### 7.2 `fib(10)` ⇒ 输出 `55`（手写汇编，验证 CALL/RET）

```asm
; 入口
        PUSH 10
        CALL fib 1
        PRINT
        HALT

; int fib(int n)   ; n 在 slot0（BP+0）
fib:
        LOAD  0          ; n
        PUSH  2
        LT               ; n < 2 ?
        JNZ   L_base
        LOAD  0
        PUSH  1
        SUB              ; n-1
        CALL  fib 1
        LOAD  0
        PUSH  2
        SUB              ; n-2
        CALL  fib 1
        ADD
        RET
L_base:
        LOAD  0
        RET
```

> 这段是**黄金参照**：将来编译器产出对不上时，`disasm` 两份一比，一眼定位 emit 错在哪条。

---

## 8. Trap 与错误码

VM 遇到以下情况立即打印诊断并 `abort()`：

| 情形 | 消息模板 |
|------|---------|
| 非法 opcode | `bad opcode 0x%02X at ip=%04zX` |
| 栈下溢 | `stack underflow at ip=%04zX (op=%s)` |
| 栈上溢 | `stack overflow at ip=%04zX` |
| 除零 / 模零 | `divide by zero at ip=%04zX` |
| 越界访存 | `mem access out of range: base=%d idx=%d at ip=%04zX` |
| ip 跑飞 | `ip out of code bounds: ip=%04zX` |
| 未对齐 HALT | （正常退出，不打诊断） |

诊断信息里**永远带 ip**——这是 VM 调试的唯一锚点。

---

## 9. 扩展预留（v1 不实现，编号先占坑）

| 段 | 未来用途 |
|----|---------|
| `0x06–0x0F` | 栈扩展：`OVER` `ROT` `PICK` `DEPTH` |
| `0x16–0x1F` | 位运算：`SHL` `SHR` `AND` `OR` `XOR` |
| `0x27–0x2F` | 浮点比较 / 字符串比较 |
| `0x36–0x3F` | 结构体字段：`GETF` `SETF` offset8 |
| `0x43–0x4F` | `SWITCH` 跳转表、`CALLI`（间接调用） |
| `0x52–0x5F` | `CALL_NATIVE`（FFI）、闭包 `CLOSURE`/`UPVALUE` |
| `0x60–0x6F` | 浮点算术 `FADD` `FMUL` ... |
| `0x70–0x7F` | 字符串 / GC：`ALLOC` `STRLEN` `STRCAT` `GC` |
| `0xF3–0xFD` | 文件 I/O：`OPEN` `READ` `WRITE` `CLOSE` `ARGV` |

加指令时**只往段尾追加**，别回头重排已发布的 op 号。

---

## 10. 三处同改 Checklist

每次改 ISA，按这个顺序过一遍，缺一处就会玄学错位：

- [ ] 更新本文件（`isa.md`）
- [ ] `isa.h`：`OpCode` 枚举
- [ ] `isa.h`：`oprand_width()` 宽度表
- [ ] `isa.h`：`MNEM[]` 反汇编名表
- [ ] `vm.c`：主循环加 `case`
- [ ] `emit.c`：加 helper（如需要）
- [ ] `disasm.c`：加解码分支（如需要）
- [ ] `tests/golden/*.asm`：加一条手搓参照
- [ ] 跑全量 golden 回归

---

**版本**：v1.0 · **状态**：冻结，可按第 10 节流程演进
