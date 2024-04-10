#include "stack.h"

int stack_init(Stack *const stack, const size_t capacity) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  if (capacity == 0) {
    LOG_ERROR("Invalid capacity for stack");
    return INVALID_ARGUMENTS;
  }

  Packet *buffer = (Packet *)malloc(sizeof(Packet) * capacity);
  if (!buffer) {
    return MEMORY_ERROR;
  }
  stack->capacity = capacity;
  stack->head_index = -1;
  stack->data_buffer = buffer;
  return SUCCESS;
}

int stack_destroy(Stack *stack) {
  free(stack);
  return SUCCESS;
}

int stack_push_item(Stack *const stack, const Packet item) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  unsigned int new_head_index = stack->head_index + 1;
  if (new_head_index >= stack->capacity) {
    size_t new_capacity = stack->capacity * 2;
    stack->data_buffer =
        (Packet *)realloc(stack->data_buffer, sizeof(Packet) * new_capacity);
    if (!stack->data_buffer) {
      return MEMORY_ERROR;
    }
    stack->capacity = new_capacity;
  }
  stack->data_buffer[new_head_index] = item;
  stack->head_index = new_head_index;
  return SUCCESS;
}

int stack_pop_item(Stack *const stack, Packet *const item) {
  if (!stack || !item) {
    return MEMORY_ERROR;
  }
  if (stack->head_index == -1) {
    return RANGE_ERROR;
  }
  int status_code = SUCCESS;
  Packet *head_item = &stack->data_buffer[stack->head_index];
  memcpy(item, head_item, sizeof(Packet));
  stack->head_index -= 1;

  if (stack->head_index < stack->capacity / 2) {
    size_t new_capacity = stack->capacity / 2;
    stack->data_buffer =
        (Packet *)realloc(stack->data_buffer, sizeof(Packet) * new_capacity);
    if (!stack->data_buffer) {
      return MEMORY_ERROR;
    }
  }
  return SUCCESS;
}

void stack_print(Stack *const stack) {
  if (!stack) {
    return;
  }
  for (unsigned int i = 0; i <= stack->head_index; i++) {
    Packet *item = &stack->data_buffer[i];
  }
}

int stack_head(Stack *const stack, Packet *item) {
  if (!stack || !item) {
    return MEMORY_ERROR;
  }
  if (stack->head_index == -1) {
    return RANGE_ERROR;
  }

  Packet *head_item = &stack->data_buffer[stack->head_index];
  item = head_item;

  return SUCCESS;
}

int stack_is_empty(Stack *const stack, bool *value) {
  if (!stack) {
    return MEMORY_ERROR;
  }

  *value = stack->head_index == -1;
  return SUCCESS;
}
