#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <stdlib.h>

#include "include/error.h"

#define MAX_PAYLOAD_SIZE 1024

typedef enum {
  DATA,
  START,
  END,
} PacketType;

typedef struct {
  PacketType type;
  size_t message_length;
  size_t range_start;
  size_t range_end;
  uint8_t* payload;
} Packet;

int init_packet(Packet* packet_ptr, size_t message_length,
                size_t range_start, size_t range_end,
                uint8_t* payload);

#endif  // PACKET_H
