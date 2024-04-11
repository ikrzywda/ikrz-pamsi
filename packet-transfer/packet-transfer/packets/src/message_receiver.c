#include "message_receiver.h"

int _init_result_message_buffer(uint8_t **const output_message_buffer_ptr,
                                const ReceiverData *const receiver_data) {
  if (!output_message_buffer_ptr || *output_message_buffer_ptr != NULL ||
      !receiver_data) {
    return MEMORY_ERROR;
  }

  if (receiver_data->message_length == 0) {
    return INVALID_ARGUMENTS;
  }

  *output_message_buffer_ptr =
      (uint8_t *)malloc(sizeof(uint8_t) * receiver_data->message_length);

  if (!(*output_message_buffer_ptr)) {
    return MEMORY_ERROR;
  }

  return SUCCESS;
}

int receiver_init(ReceiverData *const receiver_data) {
  if (!receiver_data) {
    return MEMORY_ERROR;
  }
  int status_code =
      stack_init(&receiver_data->packet_stack, DEFAULT_PACKET_STACK_CAPACITY);
  if (status_code != SUCCESS) {
    return status_code;
  }
  receiver_data->state = RECEIVER_STATE_AWAITING;
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
                            const Packet *const packet) {
  if (!receiver_data || !packet) {
    return MEMORY_ERROR;
  }
  switch (packet->type) {
  case START: {
    if (receiver_data->state != RECEIVER_STATE_AWAITING) {
      return DATA_INTEGRITY_ERROR;
    }
    receiver_data->state = RECEIVER_STATE_RECEIVING;
    receiver_data->message_length = packet->message_length;
    return SUCCESS;
  }
  case DATA: {
    if (receiver_data->state != RECEIVER_STATE_RECEIVING) {
      return DATA_INTEGRITY_ERROR;
    }
    return stack_push_item(&receiver_data->packet_stack, *packet);
  }
  case END: {
    if (receiver_data->state != RECEIVER_STATE_RECEIVING) {
      return DATA_INTEGRITY_ERROR;
    }
    receiver_data->state = RECEIVER_STATE_READY;
    return SUCCESS;
  }
  }
  return INVALID_ARGUMENTS;
}

int receiver_assemble_message(uint8_t **const output_message_buffer_ptr,
                              ReceiverData *receiver_data) {
  if (!output_message_buffer_ptr || !receiver_data) {
    return MEMORY_ERROR;
  }

  if (receiver_data->state != RECEIVER_STATE_READY) {
    return INVALID_ARGUMENTS;
  }

  int status_code =
      _init_result_message_buffer(output_message_buffer_ptr, receiver_data);
  if (status_code != SUCCESS) {
    return status_code;
  }
  bool is_packet_stack_empty;
  Packet current_packet;
  while ((status_code = stack_is_empty(&receiver_data->packet_stack,
                                       &is_packet_stack_empty)) == SUCCESS &&
         !is_packet_stack_empty) {
    status_code = stack_pop_item(&receiver_data->packet_stack, &current_packet);
    if (status_code != SUCCESS) {
      return status_code;
    }
    memcpy(*output_message_buffer_ptr + current_packet.offset,
           current_packet.payload, current_packet.length);
  }
  return SUCCESS;
}
