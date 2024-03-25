#include "packet.h"

void print_packet(Packet *packet) {
  if (!packet) {
    return;
  }
  LOG_INFO("Packet type: %d", packet->type);
  LOG_INFO("Packet message length: %ld", packet->message_length);
  LOG_INFO("Packet range start: %ld", packet->range_start);
  LOG_INFO("Packet range end: %ld", packet->range_end);
}

int init_packet(Packet *packet_ptr, PacketType packet_type,
                size_t message_length, size_t range_start, size_t range_end,
                uint8_t *payload) {
  if (!packet_ptr) {
    LOG_ERROR("Packet pointer is NULL");
    return MEMORY_ERROR;
  }
  if (range_start > range_end || message_length < 1) {
    LOG_ERROR("Invalid range start, range end or message length, %zu, %zu, %zu",
              range_start, range_end, message_length);
    return DATA_INTEGRITY_ERROR;
  }
  size_t packet_length = range_end - range_start;
  if (!payload || packet_length > MAX_PAYLOAD_SIZE) {
    LOG_ERROR("Invalid payload or packet length, %p, %zu", payload,
              packet_length);
    return DATA_INTEGRITY_ERROR;
  }

  packet_ptr->type = packet_type;
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
