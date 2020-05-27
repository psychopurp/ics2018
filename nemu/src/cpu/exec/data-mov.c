#include "cpu/exec.h"

make_EHelper(mov)
{
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push)
{
  // TODO();
  if (id_dest->width == 1)
  {
    uint8_t utmp = id_dest->val;
    int8_t temp = utmp;
    id_dest->val = temp;
  }
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop)
{
  // TODO();
  rtl_pop(&t2);                //使用临时寄存器保存值
  operand_write(id_dest, &t2); //使用operand_write执行写操作

  print_asm_template1(pop);
}

make_EHelper(pusha)
{
  TODO();

  print_asm("pusha");
}

make_EHelper(popa)
{
  TODO();

  print_asm("popa");
}

make_EHelper(leave)
{
  // TODO();
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd)
{
  if (decoding.is_operand_size_16)
  {
    // TODO();
    rtl_lr_w(&t0, R_AX);
    rtl_sext(&t0, &t0, 2);
    rtl_sari(&t0, &t0, 31);
    rtl_sr_w(R_DX, &t0);
  }
  else
  {
    // TODO();
    rtl_sari(&cpu.edx, &cpu.eax, 31);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl)
{
  if (decoding.is_operand_size_16)
  {
    // TODO();
    rtl_shli(&reg_l(R_EAX), &reg_l(R_EAX), 24);
    rtl_sari(&reg_l(R_EAX), &reg_l(R_EAX), 8);
    rtl_shri(&reg_l(R_EAX), &reg_l(R_EAX), 16);
  }
  else
  {
    // TODO();
    rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx)
{
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx)
{
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea)
{
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
