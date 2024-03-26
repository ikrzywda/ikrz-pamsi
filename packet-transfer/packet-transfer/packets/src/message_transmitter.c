#include "message_transmitter.h"

int init_transmitter_data(TransmitterData *transmitter_data,
                          const uint8_t *message, const size_t message_length,
                          const size_t max_packet_length) {
  if (!transmitter_data || !message) {
    LOG_ERROR("Invalid message data or message %p %p", transmitter_data,
              message);
    return MEMORY_ERROR;
  }

  if (message_length == 0 || max_packet_length == 0) {
    LOG_ERROR("Invalid message length or max packet size");
    return DATA_INTEGRITY_ERROR;
  }

  if (packet_init_buffer(&transmitter_data->packet_buffer, message,
                         message_length, max_packet_length) != SUCCESS) {
    return MEMORY_ERROR;
  }

  transmitter_data->message_length = message_length;
  transmitter_data->max_packet_length = max_packet_length;
  transmitter_data->state = INITIALIZED;
  return SUCCESS;
}

int destroy_transmitter_data(TransmitterData *transmitter_data) {
  if (!transmitter_data) {
    return MEMORY_ERROR;
  }
  return packet_destroy_buffer(&transmitter_data->packet_buffer);
}

int packet_init_buffer(PacketBuffer *packet_buffer, const uint8_t *message,
                       const size_t message_length,
                       const size_t max_packet_length) {

  if (!packet_buffer || !message) {
    return MEMORY_ERROR;
  }
  if (message_length == 0 || max_packet_length == 0) {
    return INVALID_ARGUMENTS;
  }

  unsigned int packet_count = message_length / max_packet_length;
  size_t last_packet_length = message_length % max_packet_length;
  if (last_packet_length > 0) {
    packet_count += 1;
  }

  Packet *packets = (Packet *)malloc(sizeof(Packet) * packet_count);
  if (!packets) {
    return MEMORY_ERROR;
  }

  int status_code = SUCCESS;
  unsigned int allocated_packet_count = 0;
  for (unsigned int i; i < packet_count; i++) {
    Packet *packet = &packets[i];
    size_t offset = i * max_packet_length;
    size_t packet_length = 0;
    if (last_packet_length > 0 && i == packet_count - 1) {
      packet_length = last_packet_length;
    } else {
      packet_length = max_packet_length;
    }
    if (offset + packet_length > message_length) {
      status_code = RANGE_ERROR;
      break;
    }
    if (packet_init(packet, DATA, message_length, offset, packet_length,
                    message) != SUCCESS) {
      status_code = MEMORY_ERROR;
      break;
    }
    allocated_packet_count++;
  }

  if (status_code != SUCCESS) {
    for (unsigned i = 0; i < allocated_packet_count; i++) {
      packet_destroy(&packets[i]);
    }
    free(packets);
  }
  packet_buffer->buffer = packets;
  packet_buffer->length = packet_count;
  return status_code;
}

int packet_destroy_buffer(PacketBuffer *packet_buffer) {
  if (!packet_buffer) {
    return MEMORY_ERROR;
  }
  int status_code = SUCCESS;
  for (int i = 0; i < packet_buffer->length; i++) {
    if (packet_destroy(&packet_buffer->buffer[i]) != SUCCESS) {
      LOG_ERROR("Failed to destroy packet");
      status_code = MEMORY_ERROR;
    }
  }
  return status_code;
}
