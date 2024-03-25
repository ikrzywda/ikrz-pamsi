#include "message_transmitter.h"

int init_message_data(MessageData *message_data, uint8_t *message,
                      size_t message_length, size_t max_packet_size) {
  if (!message_data || !message) {
    LOG_ERROR("Invalid message data or message %p %p", message_data, message);
    return MEMORY_ERROR;
  }

  if (message_length == 0 || max_packet_size == 0) {
    LOG_ERROR("Invalid message length or max packet size");
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

  LOG_DEBUG("Message length: %zu, max packet size: %zu, packet count: %zu",
            message_length, max_packet_size, packet_count);

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

  LOG_DEBUG("Last max packet size: %zu", last_max_packet_size);

  for (unsigned int i = 0; i < message_data->packet_count - 1; i++) {
    size_t packet_range_start = i * max_packet_size;
    size_t packet_range_end = packet_range_start - 1;
    LOG_DEBUG("Packet range start: %zu, range end: %zu, max packet size %zu, "
              "last max packet size %zu",
              packet_range_start, packet_range_end, max_packet_size,
              last_max_packet_size);
    if (i == message_data->packet_count - 2) {
      packet_range_end = packet_range_start + last_max_packet_size - 1;
    } else {
      packet_range_end = packet_range_start + max_packet_size - 1;
    }

    LOG_DEBUG("Packet range start: %zu, range end: %zu", packet_range_start,
              packet_range_end);

    size_t payload_size = packet_range_end - packet_range_start + 1;
    LOG_DEBUG("Payload size: %zu", payload_size);
    uint8_t *payload = (uint8_t *)malloc(payload_size);
    if (!payload) {
      return MEMORY_ERROR;
    }

    LOG_DEBUG("Building packet %d with range start %zu and range end %zu", i,
              packet_range_start, payload_size - 1);

    memcpy(payload, message_data->message + packet_range_start,
           payload_size - 1);
    Packet *packet_ptr = &message_data->packet_buffer[i];
    LOG_DEBUG("Message size %zu", message_data->message_size);
    if (init_packet(packet_ptr, DATA, message_data->message_size,
                    packet_range_start, packet_range_end, payload) != SUCCESS) {
      LOG_ERROR("Failed to initialize packet %d", i);
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
