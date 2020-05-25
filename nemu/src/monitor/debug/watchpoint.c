#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static int used_next; //记录 head 中下一个使用的 watchpoint 的编号。
static WP *wptemp;    //辅助

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].oldVal = 0;
    wp_pool[i].hitNum = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
  used_next = 0;
}

/* TODO: Implement the functionality of watchpoint */
//从free链表中返回一个空闲监视点结构，arg为该监视点的表达式。成功返回true
bool new_wp(char *args)
{
  if (free_ == NULL)
  {
    assert(0);
  }
  //记录result
  WP *result = free_;
  //更新free链表
  free_ = free_->next;

  //设置新wp的相关信息
  result->NO = used_next;
  used_next++;
  result->next = NULL;     //脱离free组织，变为链表最尾端
  strcpy(result->e, args); //记录表达式字符串
  result->hitNum = 0;
  bool success = 0;
  result->oldVal = expr(result->e, &success); //计算旧值
  if (success == false)
  {
    printf("error in new_wp: expression fault %s\n", result->e);
    return false;
  }

  //更新head链表；将新wp添加到链表最尾端
  wptemp = head;
  if (wptemp == NULL)
  {
    head = result;
  }
  else
  {
    while (wptemp->next != NULL) //wptemp找到链表尾部
      wptemp = wptemp->next;
    wptemp->next = result;
  }
  printf("Success : set watchpoint %d , oldValue=%d\n", result->NO, result->oldVal);
  return true;
}

//删除监视点：将索引号为num的wp从head中删除，并添加到free中，成功返回true，否则false
bool free_wp(int num)
{
  WP *thewp = NULL; //记录要被删除的监视点
  if (head == NULL)
  {
    printf("no watchpoint now \n");
  }
  if (head->NO == num)
  {
    thewp = head;
    head = head->next; //更新head链表
  }
  else
  {
    wptemp = head;
    while (wptemp != NULL && wptemp->next != NULL)
    {
      //找到该wp
      if (wptemp->next->NO == num)
      {
        thewp = wptemp->next;
        wptemp->next = wptemp->next->next;
        break;
      }
      wptemp = wptemp->next;
    }
  }

  //在free链表中添加
  if (thewp != NULL)
  {
    thewp->next = free_;
    free_ = thewp; //更新free链表
    return true;
  }
  return false;
}

//打印所有监视点的信息
void print_wp()
{
  if (head == NULL)
  {
    printf("no watchpoint now\n");
    return;
  }
  printf("watchpoint:\n");
  printf("NO.     expr     hitTimes\n");
  wptemp = head;
  while (wptemp != NULL)
  {
    printf("%d      %s      %d\n", wptemp->NO, wptemp->e, wptemp->hitNum);
    wptemp = wptemp->next;
  }
}

//进行监视各点表达式的求值，若有发生变化的，打印相关信息，返回false
bool watch_wp()
{
  bool success;
  int result;
  if (head == NULL)
  {
    return true;
  }
  wptemp = head;
  while (wptemp != NULL)
  {
    result = expr(wptemp->e, &success);
    //若发生改变
    if (result != wptemp->oldVal)
    {
      wptemp->hitNum++; //触发次数
      printf("Hardware watchpoint %d:%s\n", wptemp->NO, wptemp->e);
      printf("Old value:%d\nNew value:%d\n\n", wptemp->oldVal, result);
      wptemp->oldVal = result; //更新oldvalue
      return false;
    }
    wptemp = wptemp->next;
  }
  return true;
}