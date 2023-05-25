#include <stdlib.h>
#include <stdio.h>

struct stack {
    int *a;
    int top;
    int size;
    int maxsize;
};

struct stack *stack_create(int maxsize);
void stack_free(struct stack *s);
int stack_size(struct stack *s);
int stack_push(struct stack *s, int value);
int stack_pop(struct stack *s);
