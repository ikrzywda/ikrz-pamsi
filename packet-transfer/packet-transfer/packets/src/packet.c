#include "packet.h"

int packet_init(Packet *const packet_ptr, const PacketType type,
                const size_t message_length, const size_t offset,
                const size_t payload_length, const uint8_t *const payload) {
  if (!packet_ptr) {
    return MEMORY_ERROR;
  }

  if (payload_length > MAX_PATH_LENGTH || message_length == 0 ||
      (payload == NULL && type == DATA)) {
    return INVALID_ARGUMENTS;
  }

  packet_ptr->type = type;
  packet_ptr->message_length = message_length;
  packet_ptr->offset = offset;
  packet_ptr->length = payload_length;

  if (payload) {
    memcpy(packet_ptr->payload, payload, payload_length);
  }

  return SUCCESS;
}
