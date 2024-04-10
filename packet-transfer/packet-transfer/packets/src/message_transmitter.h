#pragma once

#include "common.h"
#include "packet.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct {
  const uint8_t *message_ptr;
  bool *packet_send_state;
  size_t message_length;
  size_t max_payload_length;
  unsigned int packet_count;
  uint8_t payload_buffer[PAYLOAD_BUFFER_LENGTH];
} TransmitterData;

int transmitter_init(TransmitterData *transmitter_data, const uint8_t *const message, const size_t message_length,
                      const size_t max_payload_length);
int transmitter_send_data_packet(TransmitterData *const transmitter_data,
                const unsigned int packet_index, const Packet *packet);
int transmitter_send_metadata_packet(const TransmitterData *const transmitter_data, const PacketType packet_type, Packet *const packet);
int transmitter_are_all_packets_sent(const TransmitterData *const transmitter_data, bool *const are_all_sent);
int destroy_transmitter_data(TransmitterData *transmitter_data);
