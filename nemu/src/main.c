#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

int init_monitor(int, char *[]);
void ui_mainloop(int);


uint32_t expr(char *e, bool *success);

static inline void test_expr() {
  uint32_t ans=0, result =0;
  char buf[65536];
  
  FILE *fp = fopen("tools/gen-expr/input", "r");
  while (fscanf(fp, "%d", &ans) != -1 && fscanf(fp, "%[^\n]", buf) != -1) {
    printf("ans: %u - exp: %s\n", ans, buf);
    bool success = true;
    result = expr(buf, &success);
    printf("result: %u\n", result);
    if (!success || result != ans) {
      printf("False! Please debug!\n");
      break;
    } else {
      printf("True! Cong!\n");
    }
    result = 0;
    memset(buf, 0, sizeof(buf));
  }
  fclose(fp);
}



int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  // test_expr();

  return 0;
}
