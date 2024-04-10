#pragma once 

#include "common.h"
#include "stack.h"

#include <stdio.h>

#define DEFAULT_PACKET_STACK_CAPACITY 128

typedef enum ReceiverState {
  RECEIVER_STATE_AWAITING,
  RECEIVER_STATE_RECEIVING,
  RECEIVER_STATE_READY,
} ReceiverState;

typedef struct {
  Stack packet_stack;
  ReceiverState state;
  size_t message_length;
} ReceiverData;

int receiver_init(ReceiverData *const receiver_data);

int receiver_destroy(ReceiverData *const receiver_data);

int receiver_receive_packet(ReceiverData *const receiver_data, const Packet *const packet);
int receiver_assemble_message(uint8_t *const* output_message_buffer_ptr, ReceiverData *receiver_data);

