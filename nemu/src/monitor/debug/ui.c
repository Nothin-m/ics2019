#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();
/* We use the `readline' library to provide more flexibility to read from stdin.
 */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
 
  if (args == NULL) {
    cpu_exec(1);
  } else {
    int n;
    sscanf(args, "%d", &n);
    cpu_exec(n);
  }
  return 0;
}

static int cmd_info(char *args){
  char op;
  sscanf(args, "%c", &op);
  if(op=='r'){
    isa_reg_display();
  } else if (op == 'w') {
  }
  return 0;
}

static int cmd_x(char *args) { 
  if (args == NULL){
    return 0;
  }
  char *arg = strtok(args, " ");
  int n = atoi(arg);
  arg = strtok(NULL, " ");
  if (arg == NULL) return 0;

  bool success = true;

  vaddr_t addr = expr(arg, &success);

  if(success == false ){
    printf("bad expression %s \n", arg);
    return 0;
  }
  for (int i = 0; i < n; i++) {
    printf("0x%08x: ", addr);
    for (int j = 0; j < 4; j++) {
      printf("%02x ", vaddr_read(addr, 1));
      addr++;
    }
    printf("\n");
  }
  return 0;

}

static int cmd_p(char * args){
  if (args == NULL) return 0;
  bool success = true;
  uint32_t res = expr(args, &success);
  if(success){
    printf("%s  =  %d\n", args, res);
  }
  return 0;
}


static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "si [N] exec n steps",cmd_si},
  { "info", "info r/w print regs or memory",cmd_info},
  { "x", " x N EXPR", cmd_x }, 
  { "p", "p expr ", cmd_p },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
