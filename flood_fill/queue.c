#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"

struct queue* new_queue()
{
    return calloc(1, sizeof(struct queue));
}

int is_empty(struct queue* q)
{
    return q->next == NULL;
}

void push (struct queue* q, int val)
{
    struct queue* tmp;
    tmp = malloc(sizeof(struct queue));
    tmp->val  = val;

    // queue is empty
    if(q->next == NULL)
    {
        tmp->next = tmp;
    }
    // queue is not empty
    else
    {
        tmp->next = q->next->next;
        q->next->next = tmp;
    }
    q->next = tmp;
}

int pop (struct queue* q)
{
    struct queue* tmp = q->next->next;
    
    // last element
    if(q->next == tmp)
    {
        q->next = NULL;
    }
    // not last
    else
    {
        q->next->next = tmp->next;
    }

    int value = tmp->val;
    free(tmp);

    return value;
}

void destroy(struct queue* q)
{
    while(q->next)
    {
        pop(q);
    }

    free(q);
}

void print_queue(struct queue* q)
{
    printf("q = {");
    if(q->next == NULL)
    {
        printf("}\n");
    }
    else
    {
        struct queue* first = q->next;
        struct queue* last = q->next->next;

        while(last != first)
        {
            printf("%i, ", last->val);
            last = last->next;
        }

        printf("%i}\n", last->val);
    }
}
