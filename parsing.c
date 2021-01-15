// ========== I'll build my own Lisp [2020-08-10] =====

#include <stdio.h>
#include <stdlib.h>
#include "mpc/mpc.c"
#ifdef _WIN32

static char buffer[2048];
#include <string.h>
// Artificial readline
char *readline(char *prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);

    char *cpy = malloc(strlen(buffer)+1);
    
    strcpy(cpy, buffer);

    cpy[strlen(cpy) - 1] = '0';
    
    return cpy;
}

void add_to_history(char *unused){}

#else

#include <editline/readline.h>
#include <editline/history.h>

enum {INVALID_OP, VALID_OP};
enum {DIV_ZERO, INVALID_NUM};

typedef struct Eval
{
    int type;
    long num;
    int error;
}ev;

ev *eval_num(long x)
{
    ev *v = (ev *)malloc(sizeof(ev));
    v->type = VALID_OP;
    v->num = x;
    return v;
}
ev *eval_error(int x)
{
      ev *v = (ev *)malloc(sizeof(ev));
      v->type = INVALID_OP;
      v->error = x;
      return v;
}

#endif
ev *error_check(char *var)
{
    ev *v = (ev *)malloc(sizeof(ev));
    errno = 0;
    long x = strtol(var, NULL, 10);
    if(errno != ERANGE)
    { 
        return eval_num(x);
    }
    else{
        return eval_error(INVALID_NUM);
    }
return v;
}
void liprint(ev *pr)
{
    switch(pr->type)
    {   
        case 1:
            printf("%ld\n", pr->num);
            break;
        case 0:
            switch(pr->error)
            {
                case 0:
                    printf("error: Division by zero!\n");
                    break;
                case 1:
                     printf("error: Invalid number!\n");
                     break;
                default:
                     printf("Uknown error appeared\n");
                     break;
            }
            break;
        default:
            printf("none of the previous\n");
            break;
    }
}
ev *calculus(char *op, ev *lval, ev *rval)
{
  if(lval->type == INVALID_OP) return lval;
  if(rval->type == INVALID_OP) return rval;
	switch(op[0])
	{
		case '+':
            return eval_num(lval->num+rval->num);
		case '-':
            return eval_num(lval->num-rval->num);
		case '*':
            return eval_num(lval->num*rval->num);
		case '/':
            if(rval->num == 0){
              return eval_error(DIV_ZERO);
            }
            else
              return eval_num(lval->num / rval->num);
		default:
			      printf("This operature is not exist in this lisp \n");
            break;

	}
	return eval_error(INVALID_OP);
}
// Evaluation of user input
ev *evaluation(mpc_ast_t* t)
{
  ev *x = (ev *)malloc(sizeof(ev));
  if(strstr(t->tag, "number"))
  {  
    x = error_check(t->contents);
    return x;
  }

  char* op = t->children[1]->contents;
  
  x = evaluation(t->children[2]);

  for (int i = 3; i < t->children_num - 1; i++)
  {
	  x = calculus(op, x , evaluation(t->children[i]));
  }
   
  return x;
}
// Interactive prompt
int main(int argc, char **argv)
{
  // create a parser
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  // Define them with the folling language
  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                    \
        number    : /-?[0-9]+/ ;                           \
        operator  : '+' | '-' | '*' | '/' ;                \
        expr      : <number> | '(' <operator> <expr>+ ')' ; \
        lispy     : /^/ <operator> <expr>+ /$/ ;           \
      ",
      Number, Operator, Expr, Lispy);

    puts("Alang Version 1.0");
    puts("author: B. Aissa");
    puts("Type clear to clear the screen");
    puts("Press Ctrl+c to Exit");
    while(1){

        // Output our prompt
        char *input = readline("lisp> ");
        if(strcmp(input, "clear") == 0)
        {
          system("clear");
          input = readline("lisp> ");
        }
        

        // Add buffer to history
        add_history(input);

        // Attempt to parse user input
        mpc_result_t r;
        ev *response = (ev *)malloc(sizeof(ev));
        if(mpc_parse("<stdin>", input, Lispy, &r))
        {
          /* On Success print the AST */ 
          mpc_ast_t* node = r.output;

          //Evaluation of the user input
          response = evaluation(node);
          printf("--------------------------------\n");
          liprint(response);
          printf("--------------------------------\n");

          // A parsing tree 
          //mpc_ast_print(r.output);
          //mpc_ast_delete(r.output);
        }else{
          /* Otherwise print the error */
          mpc_err_print(r.error);
          mpc_err_delete(r.error);
        }

	      // free retrived buffer
	      free(input);
    }
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
}
