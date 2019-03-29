#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

typedef enum {
  PARENT,
  COUNT,
  CHILD
} TokenType;

typedef struct {
  TokenType type;
  int value;
  char name;
} Token;

#endif
