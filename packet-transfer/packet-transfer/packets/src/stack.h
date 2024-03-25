#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "packet.h"

typedef struct {
  unsigned int size;
  unsigned int capacity;
  Packet *buffer;
} Stack;

int init_stack(Stack *stack, unsigned int capacity);
int destroy_stack(Stack *stack);

int push_stack(Stack *stack, Packet packet);
int pop_stack(Stack *stack, Packet *packet);

int is_empty_stack(Stack *stack);

int increase_stack_capacity(Stack *stack);
int decrease_stack_capacity(Stack *stack);

int head_stack(Stack *stack, Packet *packet);

#endif  // STACK_H