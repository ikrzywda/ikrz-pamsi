#include "message_transmitter.h"

int init_message_data(MessageData *message_data, uint8_t *message,
                      size_t message_length, size_t max_packet_size) {
  if (!message_data || !message) {
    return MEMORY_ERROR;
  }

  if (message_length == 0 || max_packet_size == 0) {
    return DATA_INTEGRITY_ERROR;
  }

  size_t packet_count = message_length / max_packet_size;
  if (message_length % max_packet_size != 0) {
    packet_count = packet_count + 1;
  }

  Packet *packet_buffer = (Packet *)malloc(packet_count * sizeof(Packet));
  if (!packet_buffer) {
    return MEMORY_ERROR;
  }

  message_data->message = message;
  message_data->message_size = message_length;
  message_data->max_packet_size = max_packet_size;
  message_data->packet_count = packet_count;
  message_data->packet_buffer = packet_buffer;
  message_data->state = INITIALIZED;
  return SUCCESS;
}

int build_packet_buffer(MessageData *message_data) {
  if (!message_data) {
    return MEMORY_ERROR;
  }

  size_t max_packet_size = message_data->max_packet_size;
  size_t last_max_packet_size =
      message_data->message_size % message_data->packet_count;

  for (unsigned int i = 0; i < message_data->packet_count - 1; i++) {
    size_t packet_range_start = i * max_packet_size;
    size_t packet_range_end =
        (packet_range_start - 1) +
        (last_max_packet_size > 0 && i == message_data->packet_count - 2
             ? last_max_packet_size
             : max_packet_size);

    size_t payload_size = packet_range_end - packet_range_start;
    uint8_t *payload = (uint8_t *)malloc(payload_size);
    if (!payload) {
      return MEMORY_ERROR;
    }
    if (memcpy(payload, message_data->message + packet_range_start,
               payload_size) != payload) {
      free(payload);
      return GENERIC_ERROR;
    }
    Packet *packet_ptr = &message_data->packet_buffer[i];
    if (init_packet(packet_ptr, message_data->message_size, packet_range_start,
                    packet_range_end, payload) != SUCCESS) {
      free(payload);
      return GENERIC_ERROR;
    }
  }
  message_data->state = PACKET_BUFFER_READY;
  return SUCCESS;
}

int destroy_message_data(MessageData *message_data) {
  if (!message_data) {
    return MEMORY_ERROR;
  }
  for (unsigned int i = 0; i < message_data->packet_count; i++) {
    Packet *packet_ptr = &message_data->packet_buffer[i];
    if (destroy_packet(packet_ptr) != SUCCESS) {
      LOG_ERROR("Failed to destroy packet %d", i);
      return GENERIC_ERROR;
    }
  }
  return SUCCESS;
}
