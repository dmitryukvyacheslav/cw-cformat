#include <stack/stack.h>

struct stack *stack_create(int maxsize)
{
    struct stack *s = malloc(sizeof(*s));
    if (s != NULL) {
        s->a = malloc(sizeof(int) * maxsize);
        if (s->a == NULL) {
            free(s);
            return NULL;
        }
        s->size = 0;
        s->top = 0;
        s->maxsize = maxsize;
    }
    return s;
}

void stack_free(struct stack *s)
{
    free(s->a);
    free(s);
}

int stack_size(struct stack *s)
{
    return s->size;
}

int stack_push(struct stack *s, int value)
{
    if (s->top < s->maxsize) {
        s->a[s->top++] = value;
        s->size++;
        } else {
            fprintf(stderr, "Stack overflow\n");
            return -1;
        }
    return 0;
}

int stack_pop(struct stack *s)
{
    if (s->top == 0) {
    fprintf(stderr, "Stack underflow\n");
    return -1;
    }
    s->size--;
    return s->a[--s->top];
}
