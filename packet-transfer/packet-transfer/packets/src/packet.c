#include "packet.h"

int packet_init(Packet *packet_ptr, const PacketType type,
                const size_t message_length, const size_t offset,
                const size_t payload_length, const uint8_t *payload) {
  if (!packet_ptr || !payload) {
    return MEMORY_ERROR;
  }
  if (payload_length == 0 || payload_length > MAX_PATH_LENGTH ||
      message_length == 0) {
    return INVALID_ARGUMENTS;
  }

  uint8_t *payload_buffer = (uint8_t *)malloc(payload_length);
  if (!payload_buffer) {
    return MEMORY_ERROR;
  }
  memcpy(payload_buffer, payload, payload_length);
  LOG_DEBUG("Payload: %s", payload_buffer);
  packet_ptr->type = type;
  packet_ptr->message_length = message_length;
  packet_ptr->offset = offset;
  packet_ptr->length = payload_length;
  packet_ptr->payload = payload_buffer;
  return SUCCESS;
}

int packet_destroy(Packet *packet_ptr) {
  if (!packet_ptr) {
    return MEMORY_ERROR;
  }
  free(packet_ptr->payload);
  packet_ptr->payload = NULL;
  packet_ptr->message_length = 0;
  packet_ptr->offset = 0;
  packet_ptr->length = 0;
  return SUCCESS;
}

int packet_copy(Packet *destination, const Packet *source) {
  if (!destination || !source) {
    return MEMORY_ERROR;
  }

  memcpy(destination, source, sizeof(Packet));
  return SUCCESS;
}
