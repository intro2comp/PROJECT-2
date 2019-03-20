#include <stdlib.h> // malloc
#include <string.h> // strlen

#include "queue.h"
#include "stack.h"

#include "parser.h"

#define BUFFER_SIZE 128


/* Convert a string into tokens for parsing */
Token **tokenize_input(char *str)
{
  int length = strlen(str);

  Token **tokens = malloc(BUFFER_SIZE * sizeof(Token*));

  for (int i = 0; i < BUFFER_SIZE; i++)
    tokens[i] = malloc(sizeof(Token));

  int token_count = 0;

  for (int i = 0; i < length; i++) {
    switch (str[i]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        int value = str[i] - 48;

        if (token_count != 0 && tokens[token_count-1]->type == NUMBER)
          tokens[token_count - 1]->value = tokens[token_count - 1]->value * 10 + value;
        else {
          tokens[token_count]->type = NUMBER;
          tokens[token_count++]->value = value;
        }
        break;
      }
      case '(':
        tokens[token_count++]->type = OPEN_PAREN;
        break;
      case '[':
        tokens[token_count++]->type = OPEN_BRACKET;
        break;
      case ')':
        tokens[token_count++]->type = CLOSE_PAREN;
        break;
      case ']':
        tokens[token_count++]->type = CLOSE_BRACKET;
        break;
      case '+':
        tokens[token_count++]->type = ADDITION;
        break;
      case 'x':
        tokens[token_count++]->type = MULTIPLICATION;
        break;
      default:
        break;
    }
  }

  tokens[token_count] = NULL;

  return tokens;
}


/* Parse an array of tokens using the Shunting-Yard Algorithm */
Token **parse_expression(Token **tokens)
{
  Stack *operator_stack = stack_init(128);
  Queue *output_queue = queue_init(128);

  int token_index = 0;
  Token *token = tokens[0];

  while (token) {
    switch (token->type) {
      case NUMBER:
        queue_push(output_queue, token_index);
        break;
      case ADDITION: {
        if (!stack_is_empty(operator_stack)) {
          int add_token_index = stack_peek(operator_stack);
          Token *add_token = tokens[add_token_index];
          TokenType add_token_type = add_token->type;

          if (add_token_type == MULTIPLICATION)
            while (!stack_is_empty(operator_stack))
              queue_push(output_queue, stack_pop(operator_stack));
        }
        stack_push(operator_stack, token_index);
        break;
      }
      case MULTIPLICATION:
        stack_push(operator_stack, token_index);
        break;
      case OPEN_PAREN:
        stack_push(operator_stack, token_index);
        break;
      case CLOSE_PAREN:
        while (!stack_is_empty(operator_stack) && tokens[stack_peek(operator_stack)]->type != OPEN_PAREN)
          queue_push(output_queue, stack_pop(operator_stack));
        if (!stack_is_empty(operator_stack) && tokens[stack_peek(operator_stack)]->type == OPEN_PAREN)
          stack_pop(operator_stack);
        break;
      default:
        break;
    }

    token = tokens[++token_index];
  }

  while (!stack_is_empty(operator_stack))
    queue_push(output_queue, stack_pop(operator_stack));

  Token **output_tokens = malloc(BUFFER_SIZE * sizeof(Token*));

  int output_size = output_queue->size;

  for (int i = 0; i < output_size; i++)
    output_tokens[i] = tokens[queue_pop(output_queue)];

  output_tokens[output_size] = NULL;

  free(operator_stack);
  free(output_queue);

  return output_tokens;
}
