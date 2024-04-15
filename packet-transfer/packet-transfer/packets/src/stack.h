#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "packet.h"

typedef struct {
  int head_index;
  unsigned int capacity;
  Packet *data_buffer;
} Stack;


int stack_init(Stack *const stack, const size_t capacity);
int stack_destroy(Stack *stack);
int stack_push_item(Stack *const stack, const Packet item);
int stack_pop_item(Stack *const stack, Packet *const item);
int stack_head(Stack *const stack, Packet *const item);
int stack_is_empty(Stack *const stack, bool *value);

