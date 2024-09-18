#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(auipc){
  rtl_add(&id_dest->val, &cpu.pc, &id_src->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template2(auipc);
}



make_EHelper(imm){
  switch(decinfo.isa.instr.funct3){
    case 0:  // addi
      rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);
      print_asm_template2(addi);
      break;
  }
}




make_EHelper(reg){
  switch (decinfo.isa.instr.funct3) {
    case 0:  // add  &&  sub  &&  mul
      if (decinfo.isa.instr.funct7 == 0b0000000) {        // add
        rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
        rtl_sr(id_dest->reg, &id_dest->val, 4);
        print_asm_template3(add);
      } else if (decinfo.isa.instr.funct7 == 0b0100000) {  // sub
        rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
        rtl_sr(id_dest->reg, &id_dest->val, 4);
        print_asm_template3(sub);
      } else if (decinfo.isa.instr.funct7 == 0b0000001) {  // mul
        rtl_imul_lo(&id_dest->val, &id_src->val, &id_src2->val);
        rtl_sr(id_dest->reg, &id_dest->val, 4);
        print_asm_template3(mul);
      }
      break;
     case 4:  // xor&&div
      if (decinfo.isa.instr.funct7 == 0b0000000) {  // xor
        rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
        rtl_sr(id_dest->reg, &id_dest->val, 4);
        print_asm_template3(xor);
      } else if (decinfo.isa.instr.funct7 == 0b0000001) {  // div
        rtl_idiv_q(&id_dest->val, &id_src->val, &id_src2->val);
        rtl_sr(id_dest->reg, &id_dest->val, 4);
        print_asm_template3(div);
      }
      break;
  }
}