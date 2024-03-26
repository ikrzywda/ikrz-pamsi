#ifndef MESSAGE_TRANSIMITTER_H
#define MESSAGE_TRANSIMITTER_H

#include "common.h"
#include "packet.h"

#include <stdint.h>
#include <stdio.h>


typedef enum {
  INITIALIZED,
  PACKET_BUFFER_READY,
  PACKET_BUFFER_SERIALIZED,
} TransmitterDataState;

typedef struct {
  unsigned int length;
  Packet *buffer;
} PacketBuffer;

typedef struct {
  PacketBuffer packet_buffer;
  TransmitterDataState state;
  size_t message_length;
  size_t max_packet_length;
} TransmitterData;

int init_transmitter_data(TransmitterData *transmitter_data, uint8_t *message, size_t message_length,
                      size_t max_packet_length);

int packet_init_buffer(PacketBuffer *packet_buffer, const uint8_t *message, const size_t message_length, const size_t max_packet_length);
int packet_destroy_buffer(PacketBuffer *packet_buffer);
int destroy_transmitter_data(TransmitterData *transmitter_data);

#endif  // MESSAGE_TRANSIMITTER_H
