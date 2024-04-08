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

/**
 * Initialize stack
 * 
 * @param stack pointer to an already allocated memory address
 * @param capacity desired starting capacity of the stack
 * @param destroy_item_callback callback for destroying item
 * @param copy_item_callback callback for copying item
 * @return status code
*/
int stack_init(Stack *const stack, const size_t capacity);

/**
 * Destroy stack
 * 
 * @param stack pointer to a stack
 * @return status code
*/
int stack_clear(Stack *const stack);

/**
 * Free dynamically allocated stack
 * 
 * @param[inout] stack pointer to a stack
 * @return status code
 * 
*/
int stack_destroy(Stack *stack);

/**
 * Push item to stack
 * 
 * @param stack pointer to a stack
 * @param item pointer to an item to be pushed to a stack -- stack takes ownership of the item
 * @return status code
*/
int stack_push_item(Stack *const stack, const Packet item);

/**
 * Pop item from stack
 * 
 * @param stack pointer to a stack
 * @param[out] item pointer to which assign a borrowed value of a pointer to head item
 * @return status code
*/
int stack_pop_item(Stack *const stack, Packet *const item);

/**
 * Retrieve head element of a stack
 * 
 * @param stack pointer to a stack
 * @param[out] item pointer to which copy contents of head object
 * @return status code
*/
int stack_head(Stack *const stack, Packet *const item);

/**
 * Retrieve head element of a stack
 * 
 * @param stack pointer to a stack
 * @param[out] item pointer to result value
 * @return status code
*/
int stack_is_empty(Stack *const stack, bool *value);

