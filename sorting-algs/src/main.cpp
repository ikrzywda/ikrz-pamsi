#include "csv/include/reader.hpp"

#include <iostream>
#include <charconv>
#include <string_view>
#include <vector>

#include "algs/quicksort.cpp"
#include "algs/mergesort.cpp"

int main() {
  // auto reader = read_csv("sample-data/projekt2_dane.csv");
  // if (!reader.has_value()) {
  //   std::cout << "Error reading CSV file" << std::endl;
  //   std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
  //   return 1;
  // }

  // int status_code = filter_csv(reader.value(), "rating", [](const std::string &name) {
  //   float rating;
  //   if (sscanf(name.c_str(), "%f", &rating) != 1) {
  //     return false;
  //   }
  //   return rating > 4.5;
  // });

  // if (status_code != 0) {
  //   std::cout << "Error filtering CSV" << std::endl;
  //   return 1;
  // }

  // std::ostream &stream = std::cout;
  // reader.value().dump_to_stream(stream);

  std::vector<int> dupa = {213,123,4,23,4,5,12,41,2,41,5,534,423,23};
  std::vector<int> dupa_2 = {213,123,4,23,4,5,12,41,2,41,5,534,423,23};

  quicksort<int>(dupa, 0, dupa.size() - 1);
  auto out = merge_sort<int>(dupa_2);

  for (auto el : dupa) {
    std::cout << el << ' ';
  }

  std::cout << "\n";

  for (auto el : out) {
    std::cout << el << ' ';
  }

  return 0;
}
