#ifndef DRIVER_H
#define DRIVER_H

#include "common.h"
#include "packets.h"

#include <stdint.h>
#include <stdlib.h>

#define STAGE_COUNT 4

#define DEFAULT_INPUT_FILE_PATH_ENV_VAR "INPUT_FILE"

static const char *TIMING_STAGE_NAMES_STR[] = {
    "MESSAGE_GENERATED",
    "PACKET_BUFFER_INITIALIZED",
    "PACKET_BUFFER_RECEIVED",
    "MESSAGE_REASSEMBLED",
};


typedef enum {
  MESSAGE_GENERATED = 0,
  PACKET_BUFFER_INITIALIZED,
  PACKET_BUFFER_RECEIVED,
  MESSAGE_REASSEMBLED,
} TimingStage;

typedef struct {
  size_t message_length;
  size_t part_size;
  int message_offset;
  const char *input_file_path;
} CallArguments;

void print_report(const double *timed_stages);
int init_call_arguments(CallArguments *call_arguments, int argc, char **argv);
int generate_message(uint8_t const *message, const size_t message_length,
                     const int offset, const char *input_file_path);
int init_message_order_array(size_t *message_order_array, const size_t packet_buffer_length);
int send_messages_in_random_order(PacketBuffer const* packet_buffer, size_t *message_order_array, ReceiverData *const receiver_data);
int compare_messages(const uint8_t *message, const uint8_t *reassembled_message, const size_t message_length);
int benchmark_call(const CallArguments *call_arguments);


#endif // DRIVER_H
