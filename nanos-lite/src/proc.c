#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;
int current_game = 0; //记录当前运行的游戏进程号
void switch_current_game();
uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename)
{
  int i = nr_proc++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

_RegSet *schedule(_RegSet *prev)
{
  if (current != NULL)
    current->tf = prev; //保存当前进程的现场（及上下文）
  else
    current = &pcb[current_game]; //初始进程为pcb[0]

  //进程切换frequency
  static int num = 0;
  static const int frequency = 1000;
  if (current == &pcb[current_game])
    num++; //若当前运行仙剑，则记录仙剑的运行次数
  else
    current = &pcb[current_game]; //否则切换为仙剑

  if (num == frequency)
  {
    current = &pcb[1]; //切换为hello
    num = 0;
  }
  _switch(&current->as); //切换地址空间
  return current->tf;
}

void switch_current_game()
{
  current_game = 2 - current_game; //0为仙剑 2为videotest
  Log("current_game=%d", current_game);
}