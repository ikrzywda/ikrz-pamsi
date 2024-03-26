#include "stack.h"

int stack_init(Stack *const stack, const unsigned int capacity,
               DestroyItemCallback destroy_item_callback,
               CopyItemCallback copy_item_callback) {
  if (!stack || !destroy_item_callback || !copy_item_callback) {
    return MEMORY_ERROR;
  }
  if (capacity == 0) {
    return INVALID_ARGUMENTS;
  }

  void **buffer = (void **)malloc(sizeof(void *) * capacity);
  if (!buffer) {
    return MEMORY_ERROR;
  }
  stack->capacity = capacity;
  stack->head_index = 0;
  stack->data_buffer = buffer;
  stack->destroy_item_callback = destroy_item_callback;
  stack->copy_item_callback = copy_item_callback;
  return SUCCESS;
}

int stack_clear(Stack *const stack) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  for (int i = 0; i < stack->head_index; i++) {
    void *item = stack->data_buffer[i];
    if (stack->destroy_item_callback(item) != SUCCESS) {
      LOG_ERROR("Failed deallocating stack item at index %d", i);
    }
  }

  free(stack->data_buffer);
  stack->capacity = 0;
  stack->head_index = -1;
  return SUCCESS;
}

int stack_destroy(Stack *stack) {
  int status_code = stack_clear(stack);
  if (status_code != SUCCESS) {
    return status_code;
  }
  free(stack);
  return SUCCESS;
}

int stack_push_item(Stack *const stack, const void *item) {
  if (!stack) {
    return MEMORY_ERROR;
  }
  unsigned int new_head_index = stack->head_index + 1;
  if (new_head_index >= stack->capacity) {
    size_t new_capacity = stack->capacity * 2;
    stack->data_buffer =
        (void **)realloc(stack->data_buffer, sizeof(void *) * new_capacity);
    if (!stack->data_buffer) {
      return MEMORY_ERROR;
    }
    stack->capacity = new_capacity;
  }

  stack->data_buffer[new_head_index] = item;
  stack->head_index = new_head_index;
  return SUCCESS;
}

int stack_pop_item(Stack *const stack, void **const item) {
  if (!stack || !item) {
    return MEMORY_ERROR;
  }
  if (stack->head_index == -1) {
    return RANGE_ERROR;
  }
  int status_code = SUCCESS;
  void *head_item = stack->data_buffer[stack->head_index];
  if ((status_code = stack->copy_item_callback(*item, head_item)) != SUCCESS) {
    return status_code;
  }
  if ((status_code = stack->destroy_item_callback(head_item)) != SUCCESS) {
    return status_code;
  }

  stack->head_index -= 1;

  if (stack->head_index < stack->capacity / 2) {
    size_t new_capacity = stack->capacity / 2;
    stack->data_buffer =
        (void **)realloc(stack->data_buffer, sizeof(void *) * new_capacity);
    if (!stack->data_buffer) {
      return MEMORY_ERROR;
    }
  }
  return SUCCESS;
}

int stack_head(Stack *const stack, void **item) {
  if (!stack || !item) {
    return MEMORY_ERROR;
  }
  if (stack->head_index == -1) {
    return RANGE_ERROR;
  }

  void *head_item = stack->data_buffer[stack->head_index];
  *item = head_item;

  return SUCCESS;
}

int stack_is_empty(Stack *const stack, bool *value) {
  if (!stack) {
    return MEMORY_ERROR;
  }

  *value = stack->head_index == -1;
  return SUCCESS;
}
