#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
// static int cmd_p(char *args);
// static int cmd_w(char *args);
// static int cmd_d(char *args);

static struct
{
  char *name;
  char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "si [N];execute [N] instructions step by step", cmd_si},
    {"info", "info r/w; print information aboud registers or watchpoint", cmd_info},
    {"x", "x [N] [EXPR];scan the memory", cmd_x},
    // {"p", "p [EXPR]; 表达式求值", cmd_p},
    // {"w", "w [EXPR]; set the watchpoint", cmd_w},
    // {"d", "d [N]; delete the watchpoint", cmd_d}

    /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode)
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  while (1)
  {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

//单步执行
static int cmd_si(char *args)
{
  char *arg = strtok(NULL, " ");
  if (arg == NULL)
  {
    cpu_exec(1);
    return 0;
  }
  else
  {
    int num = atoi(arg);
    cpu_exec(num);
    return 0;
  }
}

//打印寄存器
static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  if (arg == NULL)
  {
    printf("Format : info r | info w\n");
    return 0;
  }
  if (strcmp("r", arg) == 0)
  {
    extern void reg_display();
    reg_display();
  }
  else if (strcmp("w", arg) == 0)
  {
  }
  return 0;
}

//扫描内存
static int cmd_x(char *args)
{
  char *arg1 = strtok(NULL, " ");
  char *arg2 = strtok(NULL, " ");
  if (arg1 == NULL || arg2 == NULL)
  {
    printf("Format: x [N] [EXPR]\n");
    return 0;
  }
  int N = atoi(arg1);
  bool success = true;
  uint32_t target_addr = expr(arg2, &success);
  // char *str;
  // uint32_t target_addr = strtol(arg2, &str, 16);

  if (success != false)
  {
    printf("Error: not a valid expr..\n");
    return 0;
  }
  printf("-- Scanning from vaddr:0x%x\n", target_addr);
  for (int i = 0; i < N; i++)
  {
    printf("0x%08x: 0x%08x\n", target_addr, vaddr_read(target_addr, 1));
    target_addr += 4;
  }
  return 0;
}
