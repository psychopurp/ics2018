#include "common.h"

extern _RegSet *do_syscall(_RegSet *r);
extern _RegSet *schedule(_RegSet *prev);
static _RegSet *do_event(_Event e, _RegSet *r)
{
  switch (e.event)
  {
  case _EVENT_SYSCALL:
    do_syscall(r);
    return schedule(r);
  case _EVENT_TRAP:
    printf("receive an event trap!\n");
    return schedule(r); //切换进程，schedule返回新进程的上下文
  case _EVENT_IRQ_TIME:
    Log("receive IRQ_TIME!\n");
    return schedule(r);
  default:
    panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void)
{
  _asye_init(do_event);
}
