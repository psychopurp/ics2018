#include "cpu/exec.h"
#include "device/port-io.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt)
{
  TODO();

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr)
{
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r)
{
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int)
{
  TODO();

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret)
{
  TODO();

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in)
{
  // TODO();
  switch (id_src->width)
  {
  case 4:
    t0 = pio_read_l(id_src->val);
    break;
  case 1:
    t0 = pio_read_b(id_src->val);
    break;
  case 2:
    t0 = pio_read_w(id_src->val);
    break;
  default:
    assert(0);
  }

  operand_write(id_dest, &t0);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out)
{
  // TODO();
  operand_write(id_dest, &id_src->val);

  switch (id_src->width)
  {
  case 4:
    pio_write_l(id_dest->val, id_src->val);
    break;
  case 1:
    pio_write_b(id_dest->val, id_src->val);
    break;
  case 2:
    pio_write_w(id_dest->val, id_src->val);
    break;
  default:
    assert(0);
  }
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
