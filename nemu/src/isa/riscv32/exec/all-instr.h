#include "cpu/exec.h"

// compute.c
make_EHelper(lui);
make_EHelper(auipc);
make_EHelper(imm);

// control.c
make_EHelper(jal);
make_EHelper(jalr);


make_EHelper(ld);
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);
