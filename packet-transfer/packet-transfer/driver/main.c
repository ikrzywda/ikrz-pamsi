#include "common.h"
#include "driver.h"
#include "packets.h"

#include <stdio.h>
#include <unistd.h>

int print_usage() {
  printf("Usage: LOG_LEVEL=<log_level> INPUT_FILE_PATH=<input_file_path> "
         "./driver <message_length> <part_length> <offset>\n");
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

  CallArguments call_arguments;
  if (init_call_arguments(&call_arguments, argc, argv) != SUCCESS) {
    return print_usage();
  }
  echo_call_arguments(&call_arguments);
  int status_code = benchmark_call(&call_arguments);
  switch (status_code) {
  case SUCCESS:
    LOG_INFO("Benchmark completed successfully");
    break;
  case MEMORY_ERROR:
    LOG_ERROR("Memory error");
    break;
  case DATA_INTEGRITY_ERROR:
    LOG_ERROR("Data integrity error");
    break;
  default:
    LOG_ERROR("Unknown error %d", status_code);
  }

  return SUCCESS;
}
