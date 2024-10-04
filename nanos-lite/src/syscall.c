#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  int res = 0;


  switch (a[0]) {
    case SYS_yield:
      _yield();
      break;
    case SYS_exit:
      _halt(a[1]);
      break;
    case SYS_write:
      res = fs_write(a[1], (void*)a[2], a[3]);
      break;
    case SYS_brk:
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  c->GPRx = res;
  return NULL;
}
