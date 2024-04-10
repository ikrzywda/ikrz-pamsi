#ifndef DRIVER_H
#define DRIVER_H

#include "common.h"
#include "packets.h"

#include <stdint.h>
#include <stdlib.h>

#define STAGE_COUNT 4

#define DEFAULT_INPUT_FILE_PATH_ENV_VAR "INPUT_FILE"


static const char *TIMING_STAGE_NAMES_STR[] = {
    "MESSAGE_GENERATED",
    "PACKET_BUFFER_INITIALIZED",
    "PACKET_BUFFER_RECEIVED",
    "MESSAGE_REASSEMBLED",
};


typedef enum {
  MESSAGE_GENERATED = 0,
  PACKET_BUFFER_INITIALIZED,
  PACKET_BUFFER_RECEIVED,
  MESSAGE_REASSEMBLED,
} TimingStage;


int benchmark_call(const int argc, char **argv);


#endif // DRIVER_H
