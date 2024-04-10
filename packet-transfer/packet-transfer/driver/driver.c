#include "driver.h"

typedef struct {
  size_t message_length;
  size_t part_size;
  int message_offset;
  const char *input_file_path;
} CallArguments;

void _print_report(const double *timed_stages) {
  LOG_INFO("Benchmark report:");
  for (size_t i = 0; i < STAGE_COUNT; i++) {
    LOG_INFO("%s: %lf", TIMING_STAGE_NAMES_STR[i], timed_stages[i]);
  }
}

void _print_usage() {
  LOG_INFO("Usage: LOG_LEVEL=<log_level> INPUT_FILE_PATH=<input_file_path> "
           "./driver <message_length> <part_length> <offset>");
}

int _init_call_arguments(CallArguments *call_arguments, int argc, char **argv) {
  if (argc != 4) {
    _print_usage();
    return INVALID_ARGUMENTS;
  }

  if (getenv(DEFAULT_INPUT_FILE_PATH_ENV_VAR)) {
    call_arguments->input_file_path = getenv(DEFAULT_INPUT_FILE_PATH_ENV_VAR);
  } else {
    LOG_ERROR("Input file path not found");
    _print_usage();
    return INVALID_ARGUMENTS;
  }

  call_arguments->message_length = atoi(argv[1]);
  call_arguments->part_size = atoi(argv[2]);
  call_arguments->message_offset = 0;
  return SUCCESS;
}

int _generate_message(uint8_t const *message, const size_t message_length,
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

int _init_message_order_array(size_t *message_order_array,
                              const unsigned int packet_count) {
  if (!message_order_array) {
    return MEMORY_ERROR;
  }

  for (unsigned int i = 0; i < packet_count; i++) {
    message_order_array[i] = i;
  }

  for (unsigned int i = 0; i < packet_count; i++) {
    unsigned int j = rand() % packet_count;
    unsigned int temp = message_order_array[i];
    message_order_array[i] = message_order_array[j];
    message_order_array[j] = temp;
  }

  return SUCCESS;
}

int _send_messages_in_random_order(
    const TransmitterData const *transmitter_data, size_t *message_order_array,
    ReceiverData *const receiver_data) {
  if (!transmitter_data || !message_order_array || !receiver_data) {
    return MEMORY_ERROR;
  }

  Packet current_packet;
  int status_code;
  if ((status_code = transmitter_send_metadata_packet(
           transmitter_data, START, &current_packet)) != SUCCESS) {
    return status_code;
  }

  if ((status_code = receiver_receive_packet(receiver_data, &current_packet)) !=
      SUCCESS) {
    return status_code;
  }

  for (size_t i = 0; i < transmitter_data->packet_count; i++) {
    if ((status_code = transmitter_send_data_packet(
             transmitter_data, message_order_array[i], &current_packet)) !=
        SUCCESS) {
      return status_code;
    }

    if ((status_code = receiver_receive_packet(receiver_data,
                                               &current_packet)) != SUCCESS) {
      return status_code;
    }
  }

  if ((status_code = transmitter_send_metadata_packet(
           transmitter_data, END, &current_packet)) != SUCCESS) {
    return status_code;
  }
  if ((status_code = receiver_receive_packet(receiver_data, &current_packet)) !=
      SUCCESS) {
    return status_code;
  }

  return SUCCESS;
}

int _compare_messages(const uint8_t *message,
                      const uint8_t *reassembled_message,
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

int benchmark_call(const int argc, char **argv) {
  CallArguments call_arguments;
  int status_code;

  if ((status_code = _init_call_arguments(&call_arguments, argc, argv)) !=
      SUCCESS) {
    return status_code;
  }

  time_t start_time;
  double timed_stages[STAGE_COUNT];
  uint8_t *message = (uint8_t *)malloc(call_arguments.message_length);
  uint8_t *reassembled_message = NULL;
  unsigned int *message_order_array = (unsigned int *)malloc(
      call_arguments.message_length * sizeof(unsigned int));
  TransmitterData transmitter_data;
  ReceiverData receiver_data;

  if (!message) {
    return MEMORY_ERROR;
  }
  start_time = time(NULL);
  if ((status_code = _generate_message(message, call_arguments.message_length,
                                       call_arguments.message_offset,
                                       call_arguments.input_file_path)) !=
      SUCCESS) {
    return status_code;
  }
  timed_stages[MESSAGE_GENERATED] = time(NULL) - start_time;

  start_time = time(NULL);
  if ((status_code = transmitter_init(&transmitter_data, message,
                                      call_arguments.message_length,
                                      call_arguments.part_size)) != SUCCESS) {
    return status_code;
  }
  timed_stages[PACKET_BUFFER_INITIALIZED] = difftime(time(NULL), start_time);

  if ((status_code = receiver_init(&receiver_data)) != SUCCESS) {
    return status_code;
  }

  if ((status_code = _init_message_order_array(
           message_order_array, transmitter_data.packet_count)) !=
      status_code) {
    return GENERIC_ERROR;
  }
  start_time = time(NULL);
  if ((status_code = _send_messages_in_random_order(
           &transmitter_data, message_order_array, &receiver_data)) !=
      SUCCESS) {
    return status_code;
  }
  timed_stages[PACKET_BUFFER_RECEIVED] = difftime(time(NULL), start_time);

  start_time = time(NULL);
  if ((status_code = receiver_assemble_message(&reassembled_message,
                                               &receiver_data)) != SUCCESS) {
    return status_code;
  }
  timed_stages[MESSAGE_REASSEMBLED] = difftime(time(NULL), start_time);

  _print_report(timed_stages);

  FILE *output_file = fopen("output.bin", "w");
  if (!output_file) {
    return MEMORY_ERROR;
  }
  for (size_t i = 0; i < call_arguments.message_length; i++) {
    fprintf(output_file, "%c", reassembled_message[i]);
  }

  return _compare_messages(message, reassembled_message,
                           call_arguments.message_length);
}
