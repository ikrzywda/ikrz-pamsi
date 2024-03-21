#ifndef MESSAGE_RECEIVER_H
#define MESSAGE_RECEIVER_H

#include "stack.h"

typedef enum ReceiverState {
  RECEIVER_INITIALIZED,
  RECEIVER_PACKET_BUFFER_READY,
  RECEIVER_PACKET_BUFFER_DESERIALIZED,
} ReceiverState;

typedef struct {
  const char *output_file_path;
  Stack packet_stack;
} ReceiverData;

int init_receiver_data(ReceiverData *receiver_data, const char *output_file_path);
int destroy_receiver_data(ReceiverData *receiver_data);

int receive_packet(ReceiverData *receiver_data, const Packet *packet);
int deserialize_packet_buffer(ReceiverData *receiver_data);

#endif  // MESSAGE_RECEIVER_H
