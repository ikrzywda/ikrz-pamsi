#ifndef MESSAGE_RECEIVER_H
#define MESSAGE_RECEIVER_H

#include "common.h"
#include "stack.h"

#include <stdio.h>

typedef enum ReceiverState {
  RECEIVER_INITIALIZED,
  RECEIVER_PACKET_BUFFER_READY,
  RECEIVER_PACKET_BUFFER_DESERIALIZED,
} ReceiverState;

typedef struct {
  Stack packet_stack;
} ReceiverData;

int init_receiver_data(ReceiverData *receiver_data);
int destroy_receiver_data(ReceiverData *receiver_data);

int receive_packet(ReceiverData *receiver_data, Packet packet);
int assemble_message(uint8_t *output_message_buffer_ptr, ReceiverData *receiver_data);

#endif  // MESSAGE_RECEIVER_H
