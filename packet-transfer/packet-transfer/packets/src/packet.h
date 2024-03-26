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

int packet_init(Packet* packet_ptr, const PacketType type, const size_t message_length,
                const size_t offset, const size_t payload_length,
                const uint8_t* payload);
int packet_destroy(Packet* packet_ptr);
int packet_copy(Packet *destination, const Packet *source);

#endif  // PACKET_H
