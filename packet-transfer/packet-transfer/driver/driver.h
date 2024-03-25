#ifndef DRIVER_H
#define DRIVER_H

#include "packets.h"

#include <stdint.h>
#include <stdlib.h>

int send_packets_in_random_order(ReceiverData *receiver_data, uint8_t *message, size_t message_length);

#endif // DRIVER_H
