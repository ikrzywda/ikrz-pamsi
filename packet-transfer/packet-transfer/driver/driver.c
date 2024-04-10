#include "driver.h"

typedef struct {
  size_t message_length;
  size_t part_size;
  int message_offset;
  const char *input_file_path;
  const char *generated_message_path;
  const char *reassembled_message_path;
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
  if (argc != 5) {
    _print_usage();
    return INVALID_ARGUMENTS;
  }

  if (!call_arguments) {
    return MEMORY_ERROR;
  }


  call_arguments->message_length = atoi(argv[1]);
  call_arguments->part_size = atoi(argv[2]);
  call_arguments->message_offset = atoi(argv[3]);
  call_arguments->input_file_path = argv[4];
  call_arguments->generated_message_path = getenv(GENERATED_INPUT_PATH_ENV_VAR);
  call_arguments->reassembled_message_path = getenv(REASSEMBLED_PATH_ENV_VAR); 
  LOG_DEBUG("Message length: %zu, Part size: %zu, Offset: %d, Input file path: %s, Generated message path: %s, Reassembled message path: %s",
            call_arguments->message_length, call_arguments->part_size,
            call_arguments->message_offset, call_arguments->input_file_path, call_arguments->generated_message_path, call_arguments->reassembled_message_path);
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
  LOG_INFO("File size: %zu", file_size);

  uint8_t *buffer = (uint8_t *)malloc(file_size * sizeof(uint8_t));
  if (!buffer) {
    return MEMORY_ERROR;
  }

  if (fread(buffer, 1, file_size, input_file) != file_size) {
    LOG_ERROR("Failed to read file");
    return GENERIC_ERROR;
  }
  fclose(input_file);

  unsigned int iteration_count = message_length / file_size;
  unsigned int remaining_bytes = message_length % file_size;

  LOG_DEBUG("Iteration count: %u, %u", iteration_count, remaining_bytes);

  size_t offset_bytes = 0;
  for (unsigned int i = 0; i < iteration_count; i++) {
    LOG_DEBUG("Copying %zu bytes", file_size);
    memcpy(message + offset_bytes, buffer, file_size);
    offset_bytes += file_size;
  }

  // If there are remaining bytes to copy
  if (remaining_bytes > 0) {
    LOG_DEBUG("Copying remaining %zu bytes", remaining_bytes);
    memcpy(message + offset_bytes, buffer, remaining_bytes);
  }

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

  FILE *generated_input_file_path = fopen(call_arguments.generated_message_path, "w");
  if (!generated_input_file_path) {
    return MEMORY_ERROR;
  }
  fwrite(message, 1, call_arguments.message_length, generated_input_file_path);
  fclose(generated_input_file_path);

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

  FILE *output_file = fopen(call_arguments.reassembled_message_path, "w");
  if (!output_file) {
    return MEMORY_ERROR;
  }

  fwrite(reassembled_message, 1, call_arguments.message_length, output_file);
  fclose(output_file);

  return SUCCESS;
}
