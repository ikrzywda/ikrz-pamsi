#include "../include/checkers_client.h"

int main(int argc, char **argv) {
  Checkers::Client::ClientConfig config(argc, argv);
  Checkers::Client::Net::Client::run(config);
  return 0;
}
