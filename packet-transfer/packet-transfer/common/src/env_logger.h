
#pragma once

#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Logger __ENV_LOGGER;


#define LOG(level, ...) do { \
  char __LOG_MESSAGE_BUFFER[MAX_LOG_MESSAGE_LENGTH]; \
  snprintf(__LOG_MESSAGE_BUFFER, MAX_LOG_MESSAGE_LENGTH, __VA_ARGS__); \
  log_message(&__ENV_LOGGER, level, __LOG_MESSAGE_BUFFER); \
} while (0)

#define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)


int init_env_logger();
