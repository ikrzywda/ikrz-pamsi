
#ifndef ENV_LOGGER_H
#define ENV_LOGGER_H

#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Logger __ENV_LOGGER;

#define LOG_DEBUG(message) log_message(&__ENV_LOGGER, LOG_LEVEL_DEBUG, message)
#define LOG_INFO(message) log_message(&__ENV_LOGGER, LOG_LEVEL_INFO, message)
#define LOG_WARNING(message) log_message(&__ENV_LOGGER, LOG_LEVEL_WARNING, message)
#define LOG_ERROR(message) log_message(&__ENV_LOGGER, LOG_LEVEL_ERROR, message)

int init_env_logger();

#endif
