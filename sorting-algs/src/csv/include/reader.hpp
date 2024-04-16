#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <numeric>

struct CSVReader {
  std::string filename;
  std::vector<std::vector<std::string>> data;
  char delimiter;

  std::optional<unsigned int> get_column_index(const std::string &column_name) const;
  int dump_to_stream(std::ostream &stream) const;
};

std::optional<CSVReader> read_csv(const std::string &filename, char delimiter = ',');
int filter_csv(CSVReader &reader, const std::string &column, const std::function<bool(const std::string &)> &predicate);
