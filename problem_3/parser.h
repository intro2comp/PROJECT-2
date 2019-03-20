#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

typedef enum {
  NUMBER,
  OPEN_PAREN,
  CLOSE_PAREN,
  OPEN_BRACKET,
  CLOSE_BRACKET,
  ADDITION,
  MULTIPLICATION
} TokenType;

typedef struct {
  TokenType type;
  int value;
} Token;


void print_token_array(Token **tokens);

Token **tokenize_input(char *str);
Token **parse_expression(Token **tokens);

#endif
