#include "driver.h"

int log_timing_stage(DriverData *driver_data, TimingStage stage, int start) {
  if (!driver_data) {
    return MEMORY_ERROR;
  }

  if (start) {
    time(&driver_data->stage_started_times[stage]);
  } else {
    time(&driver_data->stage_finished_times[stage]);
  }
  return SUCCESS;
}

int generate_message(uint8_t *message, const char *input_file_path,
                     size_t message_length, int offset) {
  if (!message) {
    return MEMORY_ERROR;
  }

  uint8_t buffer = (uint8_t *)malloc(message_length);
  if (!buffer) {
    return MEMORY_ERROR;
  }
  FILE *input_file = fopen(input_file_path, "r");
  if (!input_file) {
    return FILE_ERROR;
  }

  fseek(input_file, offset, SEEK_END);
  size_t page_size = ftell(input_file);
  fseek(input_file, offset, SEEK_SET);

  int page_count = page_size / message_length;
  if (page_size % message_length != 0) {
    page_count = page_count + 1;
  }

  uint8_t *page_buffer = (uint8_t *)malloc(page_size);
  if (!page_buffer) {
    return MEMORY_ERROR;
  }

  if (fread(page_buffer, 1, page_size, input_file) != page_size) {
    free(page_buffer);
    fclose(input_file);
    return FILE_ERROR;
  }
  fclose(input_file);

  for (int i = 0; i < page_count; i++) {
    size_t offset = i * page_size;
    memcpy(buffer + offset, page_buffer, page_size);
  }
  free(page_buffer);
  return SUCCESS;
}

int init_driver_data(DriverData *driver_data, size_t message_length,
                     size_t part_length, int message_offset) {
  if (!driver_data) {
    return MEMORY_ERROR;
  }

  driver_data->receiver_data = (ReceiverData *)malloc(sizeof(ReceiverData));
  if (!driver_data->receiver_data) {
    return MEMORY_ERROR;
  }

  driver_data->message_data = (MessageData *)malloc(sizeof(MessageData));
  if (!driver_data->message_data) {
    return MEMORY_ERROR;
  }

  driver_data->message_length = message_length;
  driver_data->part_length = part_length;
  driver_data->message_offset = message_offset;
  return SUCCESS;
}

int benchmark(DriverData *driver_data) {
  uint8_t *message;
  LOG_INFO("Benchmarking message transmission");
  log_timing_stage(driver_data, MESSAGE_GENERATED, 1);
  if (generate_message(message, driver_data->input_file_path,
                       driver_data->message_length,
                       driver_data->message_offset) != SUCCESS) {
    return GENERIC_ERROR;
  }
  log_timing_stage(driver_data, MESSAGE_GENERATED, 0);

  // Initialize packet buffer
  log_timing_stage(driver_data, PACKET_BUFFER_INITIALIZED, 1);
  if (init_message_data(driver_data->message_data, message,
                        driver_data->message_length,
                        driver_data->part_length) != SUCCESS) {
    return GENERIC_ERROR;
  }
  log_timing_stage(driver_data, PACKET_BUFFER_INITIALIZED, 0);

  if (send_packets_in_random_order(driver_data) != SUCCESS) {
    return GENERIC_ERROR;
  }

  if (reassemble_message(driver_data->message_data) != SUCCESS) {
    return GENERIC_ERROR;
  }

  free(message);
  output_report(driver_data);

  return SUCCESS;
}

int send_packets_in_random_order(DriverData *driver_data) {
  if (!driver_data) {
    return MEMORY_ERROR;
  }

  unsigned int packet_count = driver_data->message_data->packet_count;
  int *indices = (int *)malloc(packet_count * sizeof(int));
  if (!indices) {
    return MEMORY_ERROR;
  }

  if (randomize_indices(indices, packet_count, packet_count) != SUCCESS) {
    free(indices);
    return GENERIC_ERROR;
  }

  log_timing_stage(driver_data, PACKET_BUFFER_RECEIVED, 1);
  for (unsigned int i = 0; i < packet_count; i++) {
    Packet *packet = &driver_data->message_data->packet_buffer[indices[i]];
    if (receive_packet(driver_data->receiver_data, packet) != SUCCESS) {
      free(indices);
      LOG_ERROR("Failed to receive packet %d; aborting", i);
      return GENERIC_ERROR;
    }
  }
  log_timing_stage(driver_data, PACKET_BUFFER_RECEIVED, 0);
  free(indices);
  return SUCCESS;
}

int reassemble_message(DriverData *driver_data) {
  if (!driver_data) {
    return MEMORY_ERROR;
  }

  uint8_t *output_message_buffer =
      (uint8_t *)malloc(driver_data->message_length);
  if (!output_message_buffer) {
    return MEMORY_ERROR;
  }

  log_timing_stage(driver_data, MESSAGE_REASSEMBLED, 1);
  if (assemble_message(output_message_buffer, driver_data->receiver_data) !=
      SUCCESS) {
    free(output_message_buffer);
    return GENERIC_ERROR;
  }
  log_timing_stage(driver_data, MESSAGE_REASSEMBLED, 0);

  free(output_message_buffer);
  return SUCCESS;
}

int randomize_indices(int *indices, size_t count,
                      unsigned int iteration_count) {
  if (!indices) {
    return MEMORY_ERROR;
  }

  for (unsigned int i = 0; i < count; i++) {
    indices[i] = i;
  }

  for (unsigned int i = 0; i < iteration_count; i++) {
    unsigned int index1 = rand() % count;
    unsigned int index2 = rand() % count;
    int temp = indices[index1];
    indices[index1] = indices[index2];
    indices[index2] = temp;
  }
  return SUCCESS;
}

int output_report(DriverData *driver_data) {
  if (!driver_data) {
    return MEMORY_ERROR;
  }

  time_t total_time = 0;
  for (int i = 0; i < STAGE_COUNT; i++) {
    time_t stage_time = driver_data->stage_finished_times[i] -
                        driver_data->stage_started_times[i];
    total_time += stage_time;
    LOG_INFO("Stage %s took %ld seconds", TIMING_STAGE_NAMES_STR[i],
             stage_time);
  }
  LOG_INFO("Total time: %ld seconds", total_time);
  return SUCCESS;
}
