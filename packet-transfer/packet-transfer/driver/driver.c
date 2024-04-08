#include "driver.h"

void print_report(const double *timed_stages) {
  LOG_INFO("Benchmark report:");
  for (size_t i = 0; i < STAGE_COUNT; i++) {
    LOG_INFO("%s: %lf", TIMING_STAGE_NAMES_STR[i], timed_stages[i]);
  }
}

int init_call_arguments(CallArguments *call_arguments, int argc, char **argv) {
  if (argc != 4) {
    return INVALID_ARGUMENTS;
  }

  if (getenv(DEFAULT_INPUT_FILE_PATH_ENV_VAR)) {
    call_arguments->input_file_path = getenv(DEFAULT_INPUT_FILE_PATH_ENV_VAR);
  } else {
    LOG_ERROR("Input file path not found");
    return INVALID_ARGUMENTS;
  }

  call_arguments->message_length = atoi(argv[1]);
  call_arguments->part_size = atoi(argv[2]);
  call_arguments->message_offset = 0;
  return SUCCESS;
}

int generate_message(uint8_t const *message, const size_t message_length,
                     const int offset, const char *input_file_path) {
  if (!message) {
    return MEMORY_ERROR;
  }

  FILE *input_file = fopen(input_file_path, "rb");
  if (!input_file) {
    return MEMORY_ERROR;
  }
  fseek(input_file, 0, SEEK_END);
  size_t file_size = ftell(input_file);
  fseek(input_file, offset, SEEK_SET);
  uint8_t *buffer = (uint8_t *)malloc(message_length);
  if (!buffer) {
    return MEMORY_ERROR;
  }

  if (fread(buffer, 1, message_length, input_file) != message_length) {
    return GENERIC_ERROR;
  }

  fclose(input_file);

  memcpy(message, buffer, message_length);

  return SUCCESS;
}

int init_message_order_array(size_t *message_order_array,
                             const size_t packet_buffer_length) {
  if (!message_order_array) {
    return MEMORY_ERROR;
  }

  for (size_t i = 0; i < packet_buffer_length; i++) {
    message_order_array[i] = i;
  }

  for (size_t i = 0; i < packet_buffer_length; i++) {
    size_t j = rand() % packet_buffer_length;
    size_t temp = message_order_array[i];
    message_order_array[i] = message_order_array[j];
    message_order_array[j] = temp;
  }

  return SUCCESS;
}

int send_messages_in_random_order(PacketBuffer const *packet_buffer,
                                  size_t *message_order_array,
                                  ReceiverData *const receiver_data) {
  if (!packet_buffer || !message_order_array || !receiver_data) {
    return MEMORY_ERROR;
  }

  for (size_t i = 0; i < packet_buffer->length; i++) {
    Packet packet;
    packet = packet_buffer->buffer[message_order_array[i]];
    if (receiver_receive_packet(receiver_data, packet) != SUCCESS) {
      return GENERIC_ERROR;
    }
  }

  return SUCCESS;
}

int compare_messages(const uint8_t *message, const uint8_t *reassembled_message,
                     const size_t message_length) {
  if (!message || !reassembled_message) {
    return MEMORY_ERROR;
  }

  for (size_t i = 0; i < message_length; i++) {
    if (message[i] != reassembled_message[i]) {
      LOG_ERROR("Message comparison failed at index %zu", i);
      return DATA_INTEGRITY_ERROR;
    }
  }

  return SUCCESS;
}

int benchmark_call(const CallArguments *call_arguments) {
  time_t start_time;
  double timed_stages[STAGE_COUNT];
  uint8_t *message = (uint8_t *)malloc(call_arguments->message_length);
  uint8_t *reassembled_message =
      (uint8_t *)malloc(call_arguments->message_length);
  size_t *message_order_array =
      (size_t *)malloc(call_arguments->message_length * sizeof(size_t));
  TransmitterData transmitter_data;
  ReceiverData receiver_data;

  if (!message) {
    return MEMORY_ERROR;
  }
  start_time = time(NULL);
  if (generate_message(message, call_arguments->message_length,
                       call_arguments->message_offset,
                       call_arguments->input_file_path) != SUCCESS) {
    return GENERIC_ERROR;
  }
  timed_stages[MESSAGE_GENERATED] = time(NULL) - start_time;

  start_time = time(NULL);
  if (init_transmitter_data(&transmitter_data, message,
                            call_arguments->message_length,
                            call_arguments->part_size) != SUCCESS) {
    return GENERIC_ERROR;
  }
  timed_stages[PACKET_BUFFER_INITIALIZED] = difftime(time(NULL), start_time);

  for (int i = 0; i < transmitter_data.packet_buffer.length; i++) {
    LOG_DEBUG("Packet %d: %s", i,
              transmitter_data.packet_buffer.buffer[i].payload);
  }

  if (receiver_init(&receiver_data) != SUCCESS) {
    return GENERIC_ERROR;
  }

  if (init_message_order_array(message_order_array,
                               transmitter_data.packet_buffer.length) !=
      SUCCESS) {
    return GENERIC_ERROR;
  }
  start_time = time(NULL);
  if (send_messages_in_random_order(&transmitter_data.packet_buffer,
                                    message_order_array,
                                    &receiver_data) != SUCCESS) {
    return GENERIC_ERROR;
  }
  timed_stages[PACKET_BUFFER_RECEIVED] = difftime(time(NULL), start_time);

  start_time = time(NULL);
  if (receiver_assemble_message(reassembled_message, &receiver_data) !=
      SUCCESS) {
    return GENERIC_ERROR;
  }
  LOG_DEBUG("Message reassembled %ld", start_time);
  timed_stages[MESSAGE_REASSEMBLED] = difftime(time(NULL), start_time);

  print_report(timed_stages);

  FILE *output_file = fopen("output.bin", "w");
  if (!output_file) {
    return GENERIC_ERROR;
  }
  for (size_t i = 0; i < call_arguments->message_length; i++) {
    fprintf(output_file, "%c", reassembled_message[i]);
  }

  return compare_messages(message, reassembled_message,
                          call_arguments->message_length);
}
