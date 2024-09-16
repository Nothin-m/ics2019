#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NOTEQ, TK_DECIMAL
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},  // spaces
    {"==", TK_EQ},      // equal
    {"!=", TK_NOTEQ},   // not equal
    {"\\+", '+'},       // plus
    {"\\-", '-'},       //
    {"\\*", '*'},       //
    {"/", '/'},         //

    {"\\(", '('},  //
    {"\\)", ')'},  //

    {"[0-9]+", TK_DECIMAL},  //
};


#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
            case TK_NOTYPE:
              break;
            case TK_DECIMAL:
              tokens[nr_token].type = rules[i].token_type;
              strncpy(tokens[nr_token].str, substr_start, substr_len);
              tokens[nr_token].str[substr_len] = '\0';
              nr_token++;
              break;
            default:
              tokens[nr_token].type = rules[i].token_type;
              nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int parse(Token tk) {
    char * ptr;
    switch (tk.type) {
        case TK_DECIMAL:
          return strtol(tk.str, &ptr, 10);

        default: {
          Log("cannot parse number\n");
          assert(0);
        }
    }
    return 0;
}





// result case: 1-> (exp), 0 -> wrong exp, -1 -> exp
int check_parentheses(int p, int q) {
  int result = -1;
  int layer = 0;
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    result = 1;
    for (int i = p + 1; i <= q - 1; i++) {
      if(layer < 0){
        result = -1; // 0 or -1
				break;
      }
      if (tokens[i].type == '(') layer++;
      if (tokens[i].type == ')') layer--;
    }
  }

  layer = 0;
  for (int i = p; i <= q; i++) {
    if (layer < 0) {
      result = 0;
      break;
    }
    if (tokens[i].type == '(') layer++;
    if (tokens[i].type == ')') layer--;
  }
  if (layer != 0) {
    return 0;
  }
  return result;
}


int op_precedence(int type) {
  switch (type) {
    case '*':
    case '/':
      return 3;
    case '+':
    case '-':
    case TK_EQ:
    case TK_NOTEQ:
      return 7;
  }
    return 0;
}


uint32_t findMainOp(int p, int q) {
  uint32_t op = p;
  int layer = 0;
  int precedence = 0;
  for (int i = p; i <= q; i++) {
    if (layer == 0) {
      int type = tokens[i].type;
      if (type == '(') {
        layer++;
        continue;
      }
      if (type == ')') {
        Log("Bad expression at [%d %d]\n", p, q);
        return 0;
      }
      int type_prcedence = op_precedence(type);
      if (type_prcedence >= precedence) {
        op = i;
        precedence = type_prcedence;
      }
    } else {
      if (tokens[i].type == ')') {
        layer--;
      }
      if (tokens[i].type == '(') {
        layer++;
      }
    }
    }
    if (layer != 0 || precedence == 0) {
        Log("Bad expression at [%d %d]\n", p, q);
    }
    return op;
}


uint32_t eval(int p, int q, bool* success){
  if (p > q) {
    Log("Bad expression. p>q \n");
    *success = false;
    return 0;
  } else if (p == q) {
    if (tokens[p].type != TK_DECIMAL) {
      Log("Bad expression. Single token is wrong. \n");
      *success = false;
      return 0;
    }
    return parse(tokens[p]);
  }


  int check = check_parentheses(p, q);
  if (check == 0) {
    Log("Bad expression, [%d, %d]\n", p, q);
    *success = false;
    return 0;
  } else if (check == 1) {
    return eval(p + 1, q - 1, success);
  } else {
    uint32_t op = findMainOp(p, q);
		uint32_t val1 = 0;

    if(*success==false){
    	return 0;
		}
    uint32_t val2 = eval(op+1, q, success);
    if(*success==false){
    	return 0;
		}
    switch (tokens[op].type){
	
      case '+':  return val1+val2;
      case '-':  return val1-val2;
      case '*':  return val1*val2;
      case '/': if(val2==0){  Log("Divide by 0 !\n");  *success=false; return 0;  }
        // printf("val1:%u / val2:%u\n", val1, val2);
                   return val1 / val2;
      case TK_EQ:  return val1 == val2;
      case TK_NOTEQ: return val1 != val2;

      default: {  Log("Bad expression !\n"); *success = false; return 0; }
    }
  }
}





uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  *success = true;

  return eval(0, nr_token - 1, success);
}
