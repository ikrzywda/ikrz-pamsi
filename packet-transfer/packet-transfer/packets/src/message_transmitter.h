#ifndef MESSAGE_TRANSIMITTER_H
#define MESSAGE_TRANSIMITTER_H

#include "include/error.h"
#include "packet.h"

#include <stdint.h>
#include <stdio.h>


#define MAX_PATH_LENGTH 256

typedef enum {
  INITIALIZED,
  PACKET_BUFFER_READY,
  PACKET_BUFFER_SERIALIZED,
} MessageDataState;

typedef struct {
  FILE *input_file;
  Packet *packet_buffer;
  size_t message_size;
  size_t max_packet_size;
  unsigned int packet_count;
  MessageDataState state;
} MessageData;

int init_message_data(MessageData *message_data, const char *input_file_path,
                      size_t max_packet_size);
int build_packet_buffer(MessageData *message_data);
int destroy_message_data(MessageData *message_data);

#endif  // MESSAGE_TRANSIMITTER_H
