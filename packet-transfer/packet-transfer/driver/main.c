#include "packets.h"
#include "common.h"
#include "driver.h"

#include <stdio.h>
#include <unistd.h> 

#define DEFAULT_INPUT_FILE_PATH_ENV_VAR "INPUT_FILE"

typedef struct {
  size_t message_length;
  size_t part_size;
  int message_offset;
  const char *input_file_path;
} CallArguments;

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

int print_usage() {
  printf("Usage: LOG_LEVEL=<log_level> INPUT_FILE_PATH=<input_file_path> ./driver <message_length> <part_length> <offset>\n");
  return SUCCESS;
}


int echo_call_arguments(CallArguments *call_arguments) {
  LOG_DEBUG("Message length: %zu", call_arguments->message_length);
  LOG_DEBUG("Part size: %zu", call_arguments->part_size);
  LOG_DEBUG("Message offset: %d", call_arguments->message_offset);
  LOG_DEBUG("Input file path: %s", call_arguments->input_file_path);
  return SUCCESS;
}

int main(int argc, char **argv) {
  char cwd[1024];
  init_env_logger();
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    LOG_DEBUG("Current working dir: %s", cwd);
  } else {
    LOG_ERROR("Failed to get current working dir");
  }
  LOG_DEBUG("Starting message transmitter %s", "v1.0.0");

  CallArguments call_arguments;
  if (init_call_arguments(&call_arguments, argc, argv) != SUCCESS) {
    LOG_ERROR("Invalid arguments");
    print_usage();
    return INVALID_ARGUMENTS;
  }
  echo_call_arguments(&call_arguments);

  DriverData driver_data;
  if (init_driver_data(&driver_data, call_arguments.input_file_path, call_arguments.message_length, call_arguments.part_size, call_arguments.message_offset) != SUCCESS) {
    LOG_ERROR("Failed to initialize driver data");
    return GENERIC_ERROR;
  }

  if (benchmark(&driver_data) != SUCCESS) {
    LOG_ERROR("Failed to benchmark message transmission");
    return GENERIC_ERROR;
  }

  return SUCCESS;
}


