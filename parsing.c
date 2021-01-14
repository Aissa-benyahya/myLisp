// ========== I'll build my own Lisp [2020-08-10] =====

#include <stdio.h>
#include <stdlib.h>
//#include <editline/readline.h>
//#include <editline/history.h>
#include "mpc/mpc.h"

static char buffer[2048];

#ifdef _WIN32

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

#endif
int calculus(char *op, int valx, int valy)
{
	switch(op[0])
	{
		case '+':
			return valx + valy;
		case '-':
			return valx - valy;
		case '*':
			return valx * valy;
		case '/':
			return valx / valy;
		default:
			printf("This operature is not exist in this lisp\n");

	}
	return 0;
}
// Evaluation of user input
int evaluation(mpc_ast_t* t)
{
  int x = 0;
  if(strstr(t->tag, "number")){return  atoi(t->contents);}

  char* op = t->children[1]->contents;
  
  x = evaluation(t->children[2]);

  for (int i = 3; i < t->children_num - 1; i++)
  {
	  x = calculus(op, x , evaluation(t->children[i]));
  }
   
//-----------------------------------------------------------------
 /* switch(op[0])
  {
    case '+':
      for(int i = 2; i < t->children_num-1; i++)
      {
        if(!strcmp(t->children[i]->tag, "expr|>"))
        {
          int res = evaluation(t->children[i]);
          result += res;
        }else{
          result += atoi(t->children[i]->contents);
        }
      }
      break;
    case '-':
      for(int i = 2; i < t->children_num-1; i++)
      {
        if(!strcmp(t->children[i]->tag, "expr|>"))
        {
          printf("hi inside strcmp\n");
          int res = evaluation(t->children[i]);
          if(count == 0)
          {
            result = res;
            count++;
          }
          else
          {
            result -= res;
          }
        }else{
          if(count == 0){
            result = atoi(t->children[i]->contents);
            count++;
          }
          else
            result -= atoi(t->children[i]->contents);
        }
      }
      break;
    case '*':  
      for(int i = 2; i < t->children_num-1; i++)
      {
        if(!strcmp(t->children[i]->tag, "expr|>"))
        {
          printf("hi inside strcmp\n");
          int res = evaluation(t->children[i]);
          pos *= res;
        }else{
          pos *= atoi(t->children[i]->contents);
        }
      }
      result = pos;
      break;
    case '/':
      for(int i = 2; i < t->children_num-1; i++)
      {
        if(!strcmp(t->children[i]->tag, "expr|>"))
        {
          printf("hi inside strcmp\n");
          int res = evaluation(t->children[i]);
          if(count == 0)
          {
            result = res;
            count++;
          }
          else
          {
            result /= res;
          }
        }else{
          if(count == 0)
          {
            result = atoi(t->children[i]->contents);
            count++;
          }
          else
          {
            result /= atoi(t->children[i]->contents); 
          }
        }
      }
      break;
    default:
      printf("I can't do this operation yet!!\n");
  }
*/
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
        int response = 0;
        if(mpc_parse("<stdin>", input, Lispy, &r))
        {
          /* On Success print the AST */ 
          mpc_ast_t* node = r.output;

          //Evaluation of the user input
          response = evaluation(node);
          printf("--------------------------------\n");
          printf("result = %d\n",response);
          printf("--------------------------------\n");

          // A parsing tree 
          mpc_ast_print(r.output);
          mpc_ast_delete(r.output);
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
