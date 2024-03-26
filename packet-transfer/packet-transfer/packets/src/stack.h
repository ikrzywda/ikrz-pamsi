#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "packet.h"

/**
 * Destroy callback
 * 
 * @param item pointer to item
*/
typedef int (*DestroyItemCallback)(void*);

/**
 * Copy callback
 * 
 * @param destination
 * @param source
*/
typedef int (*CopyItemCallback)(void*, const void*);
typedef struct {
  int head_index;
  unsigned int capacity;
  void **data_buffer;
  DestroyItemCallback destroy_item_callback;
  CopyItemCallback copy_item_callback;
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
int stack_init(Stack *const stack, const unsigned int capacity, DestroyItemCallback destroy_item_callback, CopyItemCallback copy_item_callback);

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
int stack_push_item(Stack *const stack, const void *item);

/**
 * Pop item from stack
 * 
 * @param stack pointer to a stack
 * @param[out] item pointer to which assign a borrowed value of a pointer to head item
 * @return status code
*/
int stack_pop_item(Stack *const stack, void **const item);

/**
 * Retrieve head element of a stack
 * 
 * @param stack pointer to a stack
 * @param[out] item pointer to which copy contents of head object
 * @return status code
*/
int stack_head(Stack *const stack, void **item);

/**
 * Retrieve head element of a stack
 * 
 * @param stack pointer to a stack
 * @param[out] item pointer to result value
 * @return status code
*/
int stack_is_empty(Stack *const stack, bool *value);

