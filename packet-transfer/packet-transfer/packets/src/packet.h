#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <stdlib.h>

#include "common.h"


typedef enum {
  DATA,
  START,
  END,
} PacketType;

typedef struct {
  PacketType type;
  size_t message_length;
  size_t offset;
  size_t length;
  uint8_t* payload;
} Packet;

int init_packet(Packet* packet_ptr, const PacketType type, const size_t message_length,
                const size_t offset, const size_t payload_length,
                const uint8_t* payload);
int destroy_packet(Packet* packet_ptr);

#endif  // PACKET_H
