#include "message_transmitter.h"

int _copy_message_slice(const uint8_t *const message,
                        const size_t message_length,
                        const size_t payload_offset,
                        const size_t payload_length,
                        uint8_t *const payload_buffer) {
  if (!message || !payload_buffer) {
    return MEMORY_ERROR;
  }
  if (message_length == 0 || payload_length == 0 ||
      payload_length > PAYLOAD_BUFFER_LENGTH) {
    return INVALID_ARGUMENTS;
  }
  if (payload_offset + payload_length > message_length) {
    return RANGE_ERROR;
  }

  memcpy(payload_buffer, message + payload_offset, payload_length);
  return SUCCESS;
}

int transmitter_send_data_packet(TransmitterData *const transmitter_data,
                                 const unsigned int packet_index,
                                 const Packet *packet) {
  if (!transmitter_data || !packet) {
    return MEMORY_ERROR;
  }
  if (packet_index > transmitter_data->packet_count) {
    return RANGE_ERROR;
  }

  if (transmitter_data->packet_send_state[packet_index]) {
    return SUCCESS;
  }

  size_t offset = packet_index * transmitter_data->max_payload_length;
  size_t length = packet_index == transmitter_data->packet_count - 1
                      ? transmitter_data->message_length %
                            transmitter_data->max_payload_length
                      : transmitter_data->max_payload_length;

  int status_code = _copy_message_slice(
      transmitter_data->message_ptr, transmitter_data->message_length, offset,
      length, transmitter_data->payload_buffer);
  if (status_code != SUCCESS) {
    return status_code;
  }

  status_code = packet_init(packet, DATA, transmitter_data->message_length,
                            offset, length, transmitter_data->payload_buffer);
  if (status_code != SUCCESS) {
    return status_code;
  }

  transmitter_data->packet_send_state[packet_index] = true;
  return SUCCESS;
}

int transmitter_send_metadata_packet(
    const TransmitterData *const transmitter_data, const PacketType packet_type,
    Packet *const packet) {
  if (!transmitter_data || !packet) {
    return MEMORY_ERROR;
  }

  if (packet_type != START || packet_type != END) {
    return INVALID_ARGUMENTS;
  }

  return packet_init(packet, packet_type, transmitter_data->message_length, 0,
                     0, NULL);
}

int transmitter_init(TransmitterData *transmitter_data,
                     const uint8_t *const message, const size_t message_length,
                     const size_t max_payload_length) {
  if (!transmitter_data || !message) {
    return MEMORY_ERROR;
  }

  if (message_length == 0 || max_payload_length == 0 ||
      max_payload_length > PAYLOAD_BUFFER_LENGTH) {
    return INVALID_ARGUMENTS;
  }

  transmitter_data->packet_count = message_length / max_payload_length +
                                   (message_length % max_payload_length > 0);
  transmitter_data->packet_send_state =
      (bool *)calloc(transmitter_data->packet_count, sizeof(bool));
  transmitter_data->message_length = message_length;
  transmitter_data->max_payload_length = max_payload_length;
  return SUCCESS;
}

int transmitter_are_all_packets_sent(
    const TransmitterData *const transmitter_data, bool *const are_all_sent) {
  if (!transmitter_data) {
    return MEMORY_ERROR;
  }
  *are_all_sent = true;
  for (unsigned int i = 0; i < transmitter_data->packet_count; i++) {
    *are_all_sent &= transmitter_data->packet_send_state[i];
  }
  return SUCCESS;
}

int destroy_transmitter_data(TransmitterData *transmitter_data) {
  if (!transmitter_data) {
    return MEMORY_ERROR;
  }
  free(transmitter_data->packet_send_state);
  return SUCCESS;
}
