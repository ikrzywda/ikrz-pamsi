#ifndef DRIVER_H
#define DRIVER_H

#include "common.h"
#include "packets.h"

#include <stdint.h>
#include <stdlib.h>

#define STAGE_COUNT 4

static const char *TIMING_STAGE_NAMES_STR[] = {
    "MESSAGE_GENERATED",
    "PACKET_BUFFER_INITIALIZED",
    "PACKET_BUFFER_RECEIVED",
    "MESSAGE_REASSEMBLED",
};

typedef enum {
  MESSAGE_GENERATED = 0,
  PACKET_BUFFER_INITIALIZED = 1,
  PACKET_BUFFER_RECEIVED = 2,
  MESSAGE_REASSEMBLED = 3,
} TimingStage;

typedef struct {
  ReceiverData *receiver_data;
  MessageData *message_data;
  const char *input_file_path;
  uint8_t *message;
  time_t stage_started_times[STAGE_COUNT];
  time_t stage_finished_times[STAGE_COUNT];
  size_t message_length;
  size_t part_length;
  int message_offset;
} DriverData;

int randomize_indices(int *indices, size_t count, unsigned int iteration_count);
int generate_message(uint8_t *message, const char *input_file_path,
                     size_t message_length, int offset);
int output_message(uint8_t *message, size_t message_length,
                   const char *output_file_path);

int init_driver_data(DriverData *driver_data, const char *input_file_path,
                     size_t message_length, size_t part_length,
                     int message_offset);
int log_timing_stage(DriverData *driver_data, TimingStage stage, int start);
int send_packets_in_random_order(DriverData *driver_data);
int reassemble_message(DriverData *driver_data);

int benchmark(DriverData *driver_data);
int output_report(DriverData *driver_data);

#endif // DRIVER_H
