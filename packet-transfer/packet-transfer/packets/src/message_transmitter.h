#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <stdio.h>

#include "packet.h"

#define MAX_PATH_LENGTH 256

typedef enum {
  INITIALIZED,
  PACKET_BUFFER_READY,
  PACKET_BUFFER_SERIALIZED,
} MessageDataState;

typedef struct {
  FILE *input_file;
  long message_size;
  unsigned int packet_size;
  unsigned int packet_count;
  Packet *packet_buffer;
  MessageDataState state;
} MessageData;

int init_message_data(MessageData *message_data, const char *input_file_path,
                      unsigned int packet_size);
int destroy_message_data(MessageData *message_data);
int build_packet_buffer(MessageData *message_data);

#endif  // MESSAGE_H
