#include "common.h"
#include "driver.h"
#include "packets.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
  init_env_logger();

  int status_code = benchmark_call(argc, argv);
  switch (status_code) {
  case SUCCESS:
    LOG_INFO("Benchmark completed successfully");
    break;
  default:
    LOG_ERROR("%s", ERROR_MESSAGE(status_code));
    break;
  }

  return SUCCESS;
}
