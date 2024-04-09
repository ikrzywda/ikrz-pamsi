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
  uint8_t payload[PAYLOAD_BUFFER_LENGTH];
  size_t message_length;
  size_t offset;
  size_t length;
  PacketType type;
} Packet;


int packet_init(Packet *const packet_ptr, const PacketType type, const size_t message_length,
                const size_t offset, const size_t payload_length,
                const uint8_t *const payload);

#endif  // PACKET_H
