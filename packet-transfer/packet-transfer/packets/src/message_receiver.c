#include "message_receiver.h"

int init_receiver_data(ReceiverData *receiver_data_ptr) {
  if (!receiver_data_ptr) {
    LOG_ERROR("Receiver data pointer is NULL");
    return MEMORY_ERROR;
  }

  if (init_stack(&receiver_data_ptr->packet_stack, DEFAULT_STACK_CAPACITY) !=
      SUCCESS) {
    LOG_ERROR("Failed to initialize packet stack");
    return MEMORY_ERROR;
  }
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
  if (push_stack(&receiver_data_ptr->packet_stack, packet) != SUCCESS) {
    return MEMORY_ERROR;
  }
  return SUCCESS;
}

int assemble_message(uint8_t *output_message_buffer_ptr,
                     ReceiverData *receiver_data) {
  if (!output_message_buffer_ptr || !receiver_data) {
    LOG_ERROR("Output message buffer or receiver data is NULL, %p, %p",
              output_message_buffer_ptr, receiver_data);
    return MEMORY_ERROR;
  }
  if (is_empty_stack(&receiver_data->packet_stack)) {
    LOG_ERROR("Packet stack is empty");
    return DATA_INTEGRITY_ERROR;
  }

  Stack *packet_stack = &receiver_data->packet_stack;
  Packet *packet_buffer = packet_stack->buffer;
  Packet packet;
  size_t message_length;
  int status_code;
  if ((status_code = head_stack(packet_stack, &packet)) != SUCCESS ||
      packet.message_length < 1) {
    LOG_ERROR("Failed to get packet from stack or message length is invalid, "
              "status code: %d, message length: %zu",
              status_code, message_length);
    return DATA_INTEGRITY_ERROR;
  }
  size_t packet_count = packet_stack->size;

  for (size_t i = 0; i < packet_count; i++) {
    if (pop_stack(packet_stack, &packet) != SUCCESS) {
      LOG_ERROR("Failed to pop packet from stack");
      return DATA_INTEGRITY_ERROR;
    }
    if (packet.range_start > message_length ||
        packet.range_end > message_length) {
      LOG_ERROR("Packet range is invalid, range start: %zu, range end: %zu, "
                "message length: %zu",
                packet.range_start, packet.range_end, message_length);
      return DATA_INTEGRITY_ERROR;
    }
    size_t payload_length = packet.range_end - packet.range_start;
    memcpy(output_message_buffer_ptr + packet.range_start, packet.payload,
           payload_length);
  }
}
