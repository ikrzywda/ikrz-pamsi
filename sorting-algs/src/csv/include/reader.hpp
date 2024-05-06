#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <numeric>

typedef std::vector<std::string> CSVRow;
typedef std::vector<CSVRow> CSVData;

struct CSVReader {
  std::string filename;
  CSVData data;
  char delimiter;

  std::optional<unsigned int> get_column_index(const std::string &column_name) const;
  int dump_to_stream(std::ostream &stream) const;
};

std::optional<CSVReader> read_csv(const std::string &filename, char delimiter = ',');
int filter_csv(CSVReader &reader, const std::string &column, const std::function<bool(const std::string &)> &predicate);

template <typename T>
std::vector<T> decode_to(CSVData &data, std::function<T(const CSVRow &)> decoder) {
  std::vector<T> decoded;
  for (const auto &row : data) {
    decoded.push_back(decoder(row));
  }
  return decoded;
}
