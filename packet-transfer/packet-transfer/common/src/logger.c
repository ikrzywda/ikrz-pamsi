#include "logger.h"

int init_logger(Logger *logger, LogLevel level) {
  if (!logger) {
    printf("Logger not initialized\n");
    return MEMORY_ERROR;
  }

  logger->level = level;
  return SUCCESS;
}

int format_message(char *buffer, LogLevel level, const char *message) {
  static char *LOG_LEVEL_STRINGS[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
  static char *LOG_LEVEL_COLORS[] = {"\033[0;36m", "\033[0;32m", "\033[0;33m", "\033[0;31m", "\033[0;35m"};
  static char *RESET_COLOR = "\033[0m";

  if (!buffer || !message) {
    printf("Invalid buffer or message\n");
    return MEMORY_ERROR;
  }

  if (level < LOG_LEVEL_DEBUG || level > LOG_LEVEL_ERROR) {
    return DATA_INTEGRITY_ERROR;
  }

  if (strlen(message) > MAX_LOG_MESSAGE_LENGTH) {
    return DATA_INTEGRITY_ERROR;
  }

  time_t current_time = time(NULL);
  struct tm *time_info = localtime(&current_time);
  if (!time_info) {
    printf("Failed to get time info\n");
    return MEMORY_ERROR;
  }

  char time_buffer[TIME_BUFFER_CAPACITY];
  strftime(time_buffer, TIME_BUFFER_CAPACITY, "%Y-%m-%d %H:%M:%S", time_info);

  char log_level_tag[LOG_LEVEL_TAG_BUFFER_CAPACITY];
  snprintf(log_level_tag, LOG_LEVEL_TAG_BUFFER_CAPACITY, "%s%s%s", LOG_LEVEL_COLORS[level], LOG_LEVEL_STRINGS[level], RESET_COLOR);
  snprintf(buffer, LOG_MESSAGE_BUFFER_CAPACITY, "[%s] [%s] %s\n", time_buffer, log_level_tag, message);
  return SUCCESS;
}

int log_message(Logger *logger, LogLevel level, const char *message) {
  if (!logger) {
    printf("Logger not initialized\n");
    return MEMORY_ERROR;
  }
  if (level < logger->level) {
    return SUCCESS;
  }

  char log_message_buffer[LOG_MESSAGE_BUFFER_CAPACITY];
  int status_code;
  if ((status_code = format_message(log_message_buffer, level, message)) != SUCCESS) {
    printf("Failed to format log message %d\n", status_code);
    return MEMORY_ERROR;
  }
  fprintf(stdout, "%s", log_message_buffer);
  return SUCCESS;
}
