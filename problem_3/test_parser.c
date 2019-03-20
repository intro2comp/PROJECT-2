#include <stdio.h>

#include "parser.h"


void print_token(Token *token)
{
  switch (token->type) {
    case NUMBER:
      printf("[%d]", token->value);
      break;
    case ADDITION:
      printf("+");
      break;
    case MULTIPLICATION:
      printf("x");
      break;
    case OPEN_PAREN:
      printf("(");
      break;
    case CLOSE_PAREN:
      printf(")");
      break;
    default:
      break;
  }
}


void print_token_array(Token **tokens)
{
  int token_index = 0;

  Token *token = tokens[0];

  while (token) {
    print_token(token);
    token = tokens[++token_index];
  }

  printf("\n");
}


int main()
{
  char *test_str = "5 x (100 x (3 + 4))";

  Token **tokens = tokenize_input(test_str);

  print_token_array(tokens);

  Token **infix_tokens = parse_expression(tokens);

  print_token_array(infix_tokens);

  return 0;
}
