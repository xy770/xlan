gcc -I../include -Wall -Wextra -g -O0 -std=c11 \
    disasm_test.c ../src/disasm.c ../src/isa.c \
    -o /tmp/disasm_test && /tmp/disasm_test
