#include "common.h"
#include "driver.h"
#include "packets.h"

#include <stdio.h>

typedef struct {
  size_t message_length;
  size_t part_size;
  int message_offset;
} CallArguments;

int init_call_arguments(CallArguments *call_arguments, int argc, char **argv) {
  if (argc != 3) {
    return INVALID_ARGUMENTS;
  }
  call_arguments->message_length = atoi(argv[1]);
  call_arguments->part_size = atoi(argv[2]);
  call_arguments->message_offset = 0;
  return SUCCESS;
}

int main(int argc, char **argv) {
  init_env_logger();

  LOG_DEBUG("Starting message transmitter %s", "v1.0.0");

  return SUCCESS;
}
