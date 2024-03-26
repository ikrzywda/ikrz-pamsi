#include "driver.h"

int output_message(uint8_t *message, size_t message_length, const char *output_file_path) {
  if (!message) {
    return MEMORY_ERROR;
  }

  FILE *output_file = fopen(output_file_path, "wb");
  if (!output_file) {
    return FILE_ERROR;
  }

  if (fwrite(message, 1, message_length, output_file) != message_length) {
    fclose(output_file);
    return FILE_ERROR;
  }
  fclose(output_file);
  return SUCCESS;

}

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

  LOG_DEBUG("Generating message from input file %s with length %zu and offset %d",
            input_file_path, message_length, offset);
  if (!message) {
    LOG_ERROR("Message buffer is NULL");
    return MEMORY_ERROR;
  }

  uint8_t *buffer = (uint8_t *)malloc(message_length);
  if (!buffer) {
    LOG_ERROR("Failed to allocate memory for message buffer");
    return MEMORY_ERROR;
  }
  FILE *input_file = fopen(input_file_path, "rb");
  if (!input_file) {
    LOG_ERROR("Failed to open input file from path %s", input_file_path);
    free(buffer);
    return FILE_ERROR;
  }

  fseek(input_file, 0, SEEK_END);
  size_t page_size = ftell(input_file);
  fseek(input_file, 0, SEEK_SET);

  int page_count = 0;
  page_count = message_length < page_size ? 1 : message_length / page_size;
  if (page_count == 1) {
    page_size = message_length;
  } else if (page_count > 0 && page_size % message_length != 0) {
    page_count = page_count + 1;
  }

  LOG_DEBUG("Message length: %zu bytes, page size: %zu bytes, page count: %d",
            message_length, page_size, page_count);
  uint8_t *page_buffer = (uint8_t *)malloc(page_size);
  if (!page_buffer) {
    return MEMORY_ERROR;
  }

  size_t bytes_read = 0;
  if ((bytes_read = fread(page_buffer, 1, page_size, input_file)) != page_size) {
    LOG_ERROR("Failed to read input file; read %zu bytes, expected %zu", bytes_read, page_size);
    free(page_buffer);
    fclose(input_file);
    return FILE_ERROR;
  }
  fclose(input_file);

  LOG_DEBUG("Read %zu bytes from input file", bytes_read);

  for (int i = 0; i < page_count; i++) {
    size_t offset = i * page_size;
    LOG_DEBUG("Copying page %d to message buffer at offset %zu", i, offset);
    memcpy(buffer + offset, page_buffer, page_size - 1);
  }
  free(page_buffer);
  message = buffer;
  return SUCCESS;
}

int init_driver_data(DriverData *driver_data, const char *input_file_path, size_t message_length,
                     size_t part_length, int message_offset) {
  LOG_DEBUG("Initializing driver data with message length %zu, part length %zu, and message offset %d",
            message_length, part_length, message_offset);
  uint8_t *message;
  if (!driver_data) {
    LOG_ERROR("Driver data is NULL");
    return MEMORY_ERROR;
  }

  log_timing_stage(driver_data, MESSAGE_GENERATED, 1);
  if (generate_message(message, input_file_path,
                       message_length,
                       message_offset) != SUCCESS) {
    LOG_ERROR("Failed to generate message");
    return GENERIC_ERROR;
  }
  log_timing_stage(driver_data, MESSAGE_GENERATED, 0);

  driver_data->receiver_data = (ReceiverData *)malloc(sizeof(ReceiverData));
  if (!driver_data->receiver_data) {
    LOG_ERROR("Failed to allocate memory for receiver data");
    return MEMORY_ERROR;
  }

  driver_data->transmitter_data = (TransmitterData *)malloc(sizeof(TransmitterData));
  if (!driver_data->transmitter_data) {
    LOG_ERROR("Failed to allocate memory for message data");
    return MEMORY_ERROR;
  }


  if (init_receiver_data(driver_data->receiver_data) != SUCCESS) { 
    LOG_ERROR("Failed to initialize receiver data");
    return GENERIC_ERROR;
  }

  LOG_DEBUG("Initializing message data with message length %zu and part length %zu",
            message_length, part_length);

  if (init_transmitter_data(driver_data->transmitter_data, message, message_length, part_length) != SUCCESS){ 
    LOG_ERROR("Failed to initialize message data");
    return GENERIC_ERROR;
  }

  driver_data->message_length = message_length;
  driver_data->part_length = part_length;
  driver_data->message_offset = message_offset;
  driver_data->message = message;
  driver_data->input_file_path = input_file_path;

  log_timing_stage(driver_data, PACKET_BUFFER_INITIALIZED, 1);
  if (init_packet_buffer(driver_data->transmitter_data) != SUCCESS) {
    LOG_ERROR("Failed to build packet buffer");
    return GENERIC_ERROR;
  }
  log_timing_stage(driver_data, PACKET_BUFFER_INITIALIZED, 0);

  return SUCCESS;
}

int benchmark(DriverData *driver_data) {
  
  LOG_INFO("Benchmarking message transmission");

  if (send_packets_in_random_order(driver_data) != SUCCESS) {
    LOG_ERROR("Failed to send packets in random order");
    return GENERIC_ERROR;
  }

  if (reassemble_message(driver_data) != SUCCESS) {
    LOG_ERROR("Failed to reassemble message");
    return GENERIC_ERROR;
  }

  char output_file_path[MAX_PATH_LENGTH];
  snprintf(output_file_path, MAX_PATH_LENGTH, "%s.reassembled", driver_data->input_file_path);
  FILE *output_file = fopen(output_file_path, "wb");
  if (!output_file) {
    return FILE_ERROR;
  }

  if (fwrite(driver_data->message, 1, driver_data->message_length, output_file) !=
      driver_data->message_length) {
    fclose(output_file);
    return FILE_ERROR;
  }

  free(driver_data->message);
  output_report(driver_data);

  return SUCCESS;
}

int send_packets_in_random_order(DriverData *driver_data) {
  if (!driver_data) {
    return MEMORY_ERROR;
  }

  unsigned int packet_count = driver_data->transmitter_data->packet_count;
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
    Packet *packet = &driver_data->transmitter_data->packet_buffer[indices[i]];
    if (receive_packet(driver_data->receiver_data, *packet) != SUCCESS) {
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
    LOG_ERROR("Driver data is NULL");
    return MEMORY_ERROR;
  }

  uint8_t *output_message_buffer =
      (uint8_t *)malloc(driver_data->message_length);
  if (!output_message_buffer) {
    LOG_ERROR("Failed to allocate memory for output message buffer");
    return MEMORY_ERROR;
  }

  log_timing_stage(driver_data, MESSAGE_REASSEMBLED, 1);
  if (assemble_message(output_message_buffer, driver_data->receiver_data) !=
      SUCCESS) {
    LOG_ERROR("Failed to assemble message");
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
