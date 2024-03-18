#include "message_transmitter.h"

#include "include/error.h"
#include "packet.h"

int init_message_data(MessageData *message_data, const char *input_file_path,
                      unsigned int packet_size) {
  if (!message_data) {
    return MEMORY_ERROR;
  }
  if (!input_file_path || sizeof(input_file_path) == 0 ||
      sizeof(input_file_path) > MAX_PATH_LENGTH || packet_size == 0 ||
      packet_size > MAX_PAYLOAD_SIZE) {
    return DATA_INTEGRITY_ERROR;
  }

  FILE *input_file = fopen(input_file_path, "rb");
  if (!input_file) {
    return FILE_ERROR;
  }

  fseek(input_file, 0, SEEK_END);
  long file_size = ftell(input_file);
  fseek(input_file, 0, SEEK_SET);

  if (file_size > MAX_PAYLOAD_SIZE) {
    fclose(input_file);
    return DATA_INTEGRITY_ERROR;
  }

  unsigned int packet_count = file_size / packet_size;
  if (file_size % packet_size != 0) {
    packet_count = packet_count + 1;
  }

  Packet *packet_buffer = (Packet *)malloc(packet_count * sizeof(Packet));
  if (!packet_buffer) {
    fclose(input_file);
    return MEMORY_ERROR;
  }

  message_data->input_file = input_file;
  message_data->message_size = file_size;
  message_data->packet_size = packet_size;
  message_data->packet_count = packet_count;
  message_data->packet_buffer = packet_buffer;
  message_data->state = INITIALIZED;
  return SUCCESS;
}

int destroy_message_data(MessageData *message_data) {
  if (!message_data) {
    return MEMORY_ERROR;
  }
  if (fclose(message_data->input_file) == EOF) {
    return FILE_ERROR;
  }
  return SUCCESS;
}

int build_packet_buffer(MessageData *message_data) {
  if (!message_data) {
    return MEMORY_ERROR;
  }

  unsigned int packet_size = message_data->packet_size;
  unsigned int last_packet_size =
      message_data->message_size % message_data->packet_count;

  for (int i = 0; i < message_data->packet_count; i++) {
    unsigned int payload_size =
        (i == message_data->packet_count - 1) ? last_packet_size : packet_size;
    uint8_t *payload = (uint8_t *)malloc(payload_size);
    if (!payload) {
      return MEMORY_ERROR;
    }
    if (fread(payload, 1, payload_size, message_data->input_file) !=
        payload_size) {
      free(payload);
      return FILE_ERROR;
    }
    Packet *packet_ptr = &message_data->packet_buffer[i];
    if (init_packet(packet_ptr, i, payload_size, payload) != SUCCESS) {
      free(payload);
      return GENERIC_ERROR;
    }
  }
  message_data->state = PACKET_BUFFER_READY;
  return SUCCESS;
}


