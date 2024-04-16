#include "csv/include/reader.hpp"

#include <iostream>

int main() {
  auto reader = read_csv("sample-data/projekt2_dane.csv");
  if (!reader.has_value()) {
    std::cout << "Error reading CSV file" << std::endl;
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    return 1;
  }

  int status_code = filter_csv(reader.value(), "rating", [](const std::string &name) {
    try {
      float raitng = std::stof(name);
      return raitng > 4.5;
    } catch (const std::invalid_argument &e) {
      return false;
    }
  });

  if (status_code != 0) {
    std::cout << "Error filtering CSV" << std::endl;
    return 1;
  }

  std::ostream &stream = std::cout;
  reader.value().dump_to_stream(stream);

  return 0;
}
