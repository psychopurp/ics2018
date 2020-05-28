#include "cpu/exec.h"

make_EHelper(test)
{
  // TODO();
  rtl_and(&t1, &id_dest->val, &id_src->val);
  t0 = 0;
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  rtl_update_ZFSF(&t1, id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and)
{
  // TODO();
  rtl_and(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);

  //modify eflags
  rtl_update_ZFSF(&t2, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(and);
}

make_EHelper(xor)
{
  // TODO();
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2); //写回寄存器

  //修改eflags
  //SF ZF
  rtl_update_ZFSF(&t2, id_dest->width);
  //CF OF <-0
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(xor);
}

make_EHelper(or)
{
  // TODO();
  rtl_or(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  //SF ZF
  rtl_update_ZFSF(&t2, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(or);
}

make_EHelper(sar)
{
  // TODO();
  if (id_dest->width == 1)
  {
    id_dest->val = (int8_t)id_dest->val;
  }
  else if (id_dest->width == 2)
  {
    id_dest->val = (int16_t)id_dest->val;
  }
  rtl_sar(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl)
{
  // TODO();
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);

  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr)
{
  // TODO();
  rtl_shr(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc)
{
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not)
{
  // TODO();
  rtl_not(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(not);
}

make_EHelper(rol)
{
  // TODO();
  int i = 0;
  rtl_mv(&t1, &id_dest->val);
  for (i = 0; i < id_src->val; ++i)
  {
    rtl_msb(&t0, &t1, id_dest->width);
    rtl_shli(&t1, &t1, 1);
    rtl_or(&t1, &t1, &t0);
  }

  operand_write(id_dest, &t1);
  rtl_set_CF(&t0);
  rtl_msb(&t1, &t1, id_dest->width);
  rtl_xor(&t0, &t1, &t0);
  rtl_set_OF(&t0);

  print_asm_template1(not);
}