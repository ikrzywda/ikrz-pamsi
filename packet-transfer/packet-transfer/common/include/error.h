#pragma once

enum PacketTransferError {
  SUCCESS = 0,
  GENERIC_ERROR = 1,
  MEMORY_ERROR = 2,
  DATA_INTEGRITY_ERROR = 3,
  INVALID_PACKET_ERROR = 4,
  FILE_ERROR = 5,
  INVALID_ARGUMENTS = 6,
  RANGE_ERROR = 7,
  NO_DATA_ERROR = 8
};

static const char *error_messages[] = {"Success",
                                       "Generic error",
                                       "Memory error",
                                       "Data integrity error",
                                       "Invalid packet error",
                                       "File error",
                                       "Invalid arguments",
                                       "Range error",
                                       "No data error"};

#define ERROR_MESSAGE(error_code) error_messages[error_code]
