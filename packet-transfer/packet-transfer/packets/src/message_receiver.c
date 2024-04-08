#include "message_receiver.h"

int receiver_init(ReceiverData *const receiver_data) {
  if (!receiver_data) {
    return MEMORY_ERROR;
  }
  int status_code =
      stack_init(&receiver_data->packet_stack, DEFAULT_PACKET_STACK_CAPACITY);
  if (status_code != SUCCESS) {
    return status_code;
  }

  return SUCCESS;
}

int receiver_destroy(ReceiverData *const receiver_data_ptr) {
  if (!receiver_data_ptr) {
    return MEMORY_ERROR;
  }
  int status_code = stack_destroy(&receiver_data_ptr->packet_stack);
  if (status_code != SUCCESS) {
    return status_code;
  }
  return SUCCESS;
}

int receiver_receive_packet(ReceiverData *const receiver_data,
                            const Packet packet) {
  if (!receiver_data) {
    return MEMORY_ERROR;
  }
  if (stack_push_item(&receiver_data->packet_stack, packet) != SUCCESS) {
    return MEMORY_ERROR;
  }
  return SUCCESS;
}

int receiver_assemble_message(uint8_t *const output_message_buffer_ptr,
                              ReceiverData *receiver_data) {
  if (!output_message_buffer_ptr || !receiver_data) {
    LOG_ERROR("Output message buffer or receiver data is NULL, %p, %p",
              output_message_buffer_ptr, receiver_data);
    return MEMORY_ERROR;
  }

  int status_code;
  bool is_empty;
  if ((status_code = stack_is_empty(&receiver_data->packet_stack, &is_empty)) !=
      SUCCESS) {
    LOG_ERROR("Failed to check if packet stack is empty");
    return status_code;
  }

  if (is_empty) {
    LOG_ERROR("Packet stack is empty");
    return DATA_INTEGRITY_ERROR;
  }

  Stack *packet_stack = &receiver_data->packet_stack;
  Packet packet;

  if ((status_code = stack_head(packet_stack, &packet)) != SUCCESS ||
      packet.message_length < 1) {
    LOG_ERROR("Failed to get packet from stack or message length is invalid, "
              "status code: %d, message length: %zu",
              status_code, packet.message_length);
    return DATA_INTEGRITY_ERROR;
  }
  size_t message_length = packet.message_length;
  size_t packet_count = packet_stack->head_index;

  for (size_t i = 0; i < packet_count; i++) {
    if (stack_pop_item(packet_stack, &packet) != SUCCESS) {
      LOG_ERROR("Failed to pop packet from stack");
      return DATA_INTEGRITY_ERROR;
    }
    if (packet.offset + packet.length > message_length) {
      return DATA_INTEGRITY_ERROR;
    }
    LOG_DEBUG(
        "Copying packet payload to output buffer, offset: %zu, length: %zu",
        packet.offset, packet.length);
    LOG_DEBUG("%s", packet.payload);
    memcpy(output_message_buffer_ptr + packet.offset, packet.payload,
           packet.length);
  }
  return SUCCESS;
}
