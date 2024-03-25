#include "packet.h"

int init_packet(Packet *packet_ptr, size_t message_length, size_t range_start,
                size_t range_end, uint8_t *payload) {
  if (!packet_ptr) {
    return MEMORY_ERROR;
  }
  if (range_start > range_end || message_length < 1) {
    return DATA_INTEGRITY_ERROR;
  }
  size_t packet_length = range_end - range_start;
  if (!payload || packet_length > MAX_PAYLOAD_SIZE ||
      sizeof(payload) != packet_length) {
    return DATA_INTEGRITY_ERROR;
  }
  packet_ptr->payload = payload;
  packet_ptr->message_length = message_length;
  packet_ptr->range_start = range_start;
  packet_ptr->range_end = range_end;
  return SUCCESS;
}

int destroy_packet(Packet *packet_ptr) {
  if (!packet_ptr) {
    return MEMORY_ERROR;
  }
  free(packet_ptr->payload);
  packet_ptr->payload = NULL;
  packet_ptr->message_length = 0;
  packet_ptr->range_start = 0;
  packet_ptr->range_end = 0;
  return SUCCESS;
}
