#include "message_receiver.h"

int receiver_init(ReceiverData const *receiver_data) {
  if (!receiver_data) {
    return MEMORY_ERROR;
  }
  int status_code =
      stack_init(&receiver_data->packet_stack, DEFAULT_PACKET_STACK_CAPACITY,
                 packet_destroy, packet_copy);
  if (status_code != SUCCESS) {
    return status_code;
  }

  return SUCCESS;
}

int receiver_destroy(ReceiverData *receiver_data_ptr) {
  if (!receiver_data_ptr) {
    return MEMORY_ERROR;
  }
  if (stack_destroy(&receiver_data_ptr->packet_stack) != SUCCESS) {
    return MEMORY_ERROR;
  }
  return SUCCESS;
}

int receiver_receive_packet(ReceiverData *receiver_data_ptr, Packet packet) {
  if (!receiver_data_ptr) {
    return MEMORY_ERROR;
  }
  if (push_stack(&receiver_data_ptr->packet_stack, packet) != SUCCESS) {
    return MEMORY_ERROR;
  }
  return SUCCESS;
}

int receiver_assemble_message(uint8_t *output_message_buffer_ptr,
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
  Packet packet;
  int status_code;

  if ((status_code = head_stack(packet_stack, &packet)) != SUCCESS ||
      packet.message_length < 1) {
    LOG_ERROR("Failed to get packet from stack or message length is invalid, "
              "status code: %d, message length: %zu",
              status_code, packet.message_length);
    return DATA_INTEGRITY_ERROR;
  }
  size_t message_length = packet.message_length;
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
  return SUCCESS;
}
