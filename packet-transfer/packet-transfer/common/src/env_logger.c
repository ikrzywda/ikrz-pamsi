#include "env_logger.h"

int init_env_logger() {
  const char *env_log_level = getenv("LOG_LEVEL");

  if (!env_log_level) {
    printf("Environment variables not set; failed to initialize env logger\n");
    return DATA_INTEGRITY_ERROR;
  }

  if (strcmp(env_log_level, "DEBUG") == 0) {
    return init_logger(&__ENV_LOGGER, LOG_LEVEL_DEBUG);
  } else if (strcmp(env_log_level, "INFO") == 0) {
    return init_logger(&__ENV_LOGGER, LOG_LEVEL_INFO);
  } else if (strcmp(env_log_level, "WARNING") == 0) {
    return init_logger(&__ENV_LOGGER, LOG_LEVEL_WARNING);
  } else if (strcmp(env_log_level, "ERROR") == 0) {
    return init_logger(&__ENV_LOGGER, LOG_LEVEL_ERROR);
  }

  return DATA_INTEGRITY_ERROR;
}
