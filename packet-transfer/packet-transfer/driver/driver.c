#include "driver.h"

int send_packets_in_random_order(ReceiverData *receiver_data,
                                 const char *input_file_path,
                                 size_t message_length) {
  int status_code = SUCCESS;
  MessageData message_data;
  if ((status_code = init_message_data(&message_data, message,
                                       message_length)) != SUCCESS) {
    return status_code;
  }
}
