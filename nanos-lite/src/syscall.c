#include "common.h"
#include "syscall.h"
#include "proc.h"

extern void naive_uload(PCB *pcb, const char *filename);

static int programBrk;

int do_yield(){
  _yield();
  return 0;
}


void do_exit(uintptr_t arg){
  _halt(arg);
}

int do_write(int fd, const void*buf, size_t count){
  if(fd==1 || fd==2){
    for(int i = 0;i < count;i++){
        _putc(((char*)buf)[i]);
    }
    return count;
  }
  return 0;
}

int do_brk(int addr){
  programBrk = addr;
  return 0;
}




_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;


  switch (a[0]) {
    case SYS_yield:
      c->GPRx = do_yield();
      break;
    case SYS_exit:
      do_exit(a[1]);
      break;
    case SYS_write:
      c->GPRx = do_write(a[1], (void *)(a[2]), a[3]);
      break;
    case SYS_brk:
      c->GPRx = do_brk(a[1]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
