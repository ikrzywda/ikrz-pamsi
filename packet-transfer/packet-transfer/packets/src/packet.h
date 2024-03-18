#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

#define MAX_PAYLOAD_SIZE 1024

typedef enum {
  DATA,
  START,
  END,
} PacketType;

typedef struct {
  uint32_t id;
  uint32_t length;
  PacketType type;
  uint8_t* payload;
} Packet;

int init_packet(Packet* packet_ptr, uint32_t id, uint32_t length,
                uint8_t* payload);

#endif  // PACKET_H
