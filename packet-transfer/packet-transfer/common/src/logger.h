#ifndef LOGGER_H
#define LOGGER_H

#include "error.h"
#include "constants.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define TIME_BUFFER_CAPACITY 32
#define LOG_LEVEL_TAG_BUFFER_CAPACITY 32
#define LOG_MESSAGE_BUFFER_CAPACITY 4096
#define MAX_LOG_MESSAGE_LENGTH 4030


typedef enum {
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
} LogLevel;

static const char *LOG_LEVEL_STRINGS[] = {
  "DEBUG",
  "INFO",
  "WARNING",
  "ERROR",
};

static const char *LOG_LEVEL_COLORS[] = {
  "\033[0;36m",  // Cyan
  "\033[0;32m",  // Green
  "\033[0;33m",  // Yellow
  "\033[0;31m",  // Red
};

static const char *LOG_RESET_COLOR = "\033[0m";

typedef struct {
  FILE *output_stream;
  LogLevel level;
} Logger;

int init_logger(Logger *logger, LogLevel level);
int format_message(char *buffer, LogLevel level, const char *message);
int log_message(Logger *logger, LogLevel level, const char *message);

#endif  // LOGGER_H
