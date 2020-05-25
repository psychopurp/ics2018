#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  int oldVal; //旧值
  char e[32]; //表达式
  int hitNum; //记录出发次数

} WP;

bool new_wp(char *args);
bool free_wp(int num);
void print_wp();
bool watch_wp();

#endif
