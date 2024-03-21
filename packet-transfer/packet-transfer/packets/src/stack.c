#include "stack.h"

int init_stack(Stack *stack, unsigned int capacity) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  if (capacity == 0) {
    return DATA_INTEGRITY_ERROR;
  }
  Packet *packet_buffer = (Packet *)malloc(capacity * sizeof(Packet));
  if (!packet_buffer) {
    return MEMORY_ERROR;
  }
  stack->capacity = capacity;
  stack->size = 0;
  stack->buffer = packet_buffer;
  return SUCCESS;
}

int destroy_stack(Stack *stack) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  for (size_t i = 0; i < stack->size; i++) {
    free(stack->buffer[i].payload);
  }
  free(stack->buffer);
  return SUCCESS;
}

int push_stack(Stack *stack, Packet packet) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  if (is_full_stack(stack)) {
    return DATA_INTEGRITY_ERROR;
  }
  stack->buffer[stack->size] = packet;
  stack->size = stack->size + 1;
  return SUCCESS;
}

int pop_stack(Stack *stack, Packet *packet) {
  if (!stack || !packet) {
    return MEMORY_ERROR;
  }
  if (is_empty_stack(stack)) {
    return DATA_INTEGRITY_ERROR;
  }
  *packet = stack->buffer[stack->size - 1];
  stack->size = stack->size - 1;
  return SUCCESS;
}

int is_empty_stack(Stack *stack) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  return stack->size == 0;
}

int is_full_stack(Stack *stack) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  return stack->size == stack->capacity;
}

int head_stack(Stack *stack, Packet *packet) {
  if (!stack || !packet) {
    return MEMORY_ERROR;
  }
  if (is_empty_stack(stack)) {
    return DATA_INTEGRITY_ERROR;
  }
  *packet = stack->buffer[stack->size - 1];
  return SUCCESS;
}
