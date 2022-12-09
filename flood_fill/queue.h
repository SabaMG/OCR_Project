#pragma once

struct queue {
    int val;
    struct queue* next;
};

struct queue* new_queue();
int is_empty(struct queue* q);
void push (struct queue* q, int val);
int pop (struct queue* q);
void destroy(struct queue* q);
void print_queue(struct queue* q);
