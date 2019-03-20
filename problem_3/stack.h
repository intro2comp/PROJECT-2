typedef struct {
  int top;
  int max_size;
  int data[];
} Stack;

Stack *stack_init(int size);
void stack_destroy(Stack *stack);

int stack_is_empty(Stack *stack);
int stack_is_full(Stack *stack);

int stack_peek(Stack *stack);
void stack_push(Stack *stack, int value);
int stack_pop(Stack *stack);


