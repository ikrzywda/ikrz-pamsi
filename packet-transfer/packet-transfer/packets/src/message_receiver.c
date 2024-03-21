#include "message_receiver.h"

int init_receiver_data(ReceiverData *receiver_data_ptr, const char *output_file_path) {
  if (!receiver_data_ptr) {
    return MEMORY_ERROR;
  }
  if (!output_file_path || sizeof(output_file_path) == 0 ||
      sizeof(output_file_path) > MAX_PATH_LENGTH) {
    return DATA_INTEGRITY_ERROR;
  }

  FILE *output_file = fopen(output_file_path, "wb");
  if (!output_file) {
    return FILE_ERROR;
  }
  fclose(output_file);

  if (init_stack(&receiver_data_ptr->packet_stack, DEFAULT_STACK_CAPACITY) != SUCCESS) {
    return MEMORY_ERROR;
  }
  receiver_data_ptr->output_file_path = output_file_path;
  return SUCCESS;
}

int destroy_receiver_data(ReceiverData *receiver_data_ptr) {
  if (!receiver_data_ptr) {
    return MEMORY_ERROR;
  }
  if (destroy_stack(&receiver_data_ptr->packet_stack) != SUCCESS) {
    return MEMORY_ERROR;
  }
  return SUCCESS;
}

int receive_packet(ReceiverData *receiver_data_ptr, Packet packet) {
  if (!receiver_data_ptr) {
    return MEMORY_ERROR;
  }
  if (is_full_stack(&receiver_data_ptr->packet_stack)) {
    return DATA_INTEGRITY_ERROR;
  }
  if (push_stack(&receiver_data_ptr->packet_stack, packet) != SUCCESS) {
    return MEMORY_ERROR;
  }
  return SUCCESS;
}


int assemble_message(uint8_t *output_message_buffer_ptr, ReceiverData *receiver_data) {
  if (!output_message_buffer_ptr || !receiver_data) {
    return MEMORY_ERROR;
  }
  if (is_empty_stack(&receiver_data->packet_stack)) {
    return DATA_INTEGRITY_ERROR;
  }

  Stack *packet_stack = &receiver_data->packet_stack;
  Packet *packet_buffer = packet_stack->buffer;
  size_t message_length;
  if (head_stack(packet_stack, &message_length) != SUCCESS || message_length < 1) {
    return DATA_INTEGRITY_ERROR;
  }
  size_t packet_count = packet_stack->size;

  for (size_t i = 0; i < packet_count; i++) {
    Packet packet;
    if (pop_stack(packet_stack, &packet) != SUCCESS) {
      return DATA_INTEGRITY_ERROR;
    }
    if (packet.range_start > message_length || packet.range_end > message_length) {
      return DATA_INTEGRITY_ERROR;
    }
    size_t payload_length = packet.range_end - packet.range_start;
    memcpy(output_message_buffer_ptr + packet.range_start, packet.payload, payload_length);
  }
}
