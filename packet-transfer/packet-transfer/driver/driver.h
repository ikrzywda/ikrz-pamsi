#pragma once

#include "common.h"
#include "packets.h"

#include <stdint.h>
#include <stdlib.h>

#define STAGE_COUNT 6

#define GENERATED_INPUT_PATH_ENV_VAR "GENERATED_INPUT_PATH"
#define REASSEMBLED_PATH_ENV_VAR "REASSEMBLED_INPUT_PATH"


static const char *TIMING_STAGE_NAMES_STR[] = {
    "MESSAGE_GENERATED",
    "PACKET_BUFFER_INITIALIZED",
    "TRANSMISSION_MEAN_TIME",
    "RECEIVAL_MEAN_TIME",
    "TRANSMISSION_DONE",
    "MESSAGE_REASSEMBLED",
};


typedef enum {
  MESSAGE_GENERATED = 0,
  PACKET_BUFFER_INITIALIZED,
  TRANSMISSION_MEAN_TIME,
  RECEIVAL_MEAN_TIME,
  TRANSMISSION_DONE,
  MESSAGE_REASSEMBLED,
} TimingStage;


int benchmark_call(int argc, char **argv);

