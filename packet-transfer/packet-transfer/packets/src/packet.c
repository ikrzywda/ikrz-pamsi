#include "packet.h"

#include "include/error.h"

int init_packet(Packet* packet_ptr, uint32_t id, uint32_t length,
                uint8_t* payload) {
  if (!packet_ptr) {
    return MEMORY_ERROR;
  }
  if (!payload || length == 0 || length > MAX_PAYLOAD_SIZE ||
      sizeof(payload) != length) {
    return DATA_INTEGRITY_ERROR;
  }
  packet_ptr->id = id;
  packet_ptr->length = length;
  packet_ptr->payload = payload;
  return SUCCESS;
}
