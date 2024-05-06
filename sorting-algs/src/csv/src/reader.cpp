#include "../include/reader.hpp"



std::optional<unsigned int> CSVReader::get_column_index(const std::string &column_name) const {
  for (size_t i = 0; i < data[0].size(); i++) {
    if (data[0][i] == column_name) {
      return i;
    }
  }

  return std::nullopt;
}

int CSVReader::dump_to_stream(std::ostream &stream) const {
  for (const auto &row : data) {
    for (size_t i = 0; i < row.size(); i++) {
      stream << row[i];
      if (i < row.size() - 1) {
        stream << delimiter;
      }
    }
    stream << std::endl;
  }

  return 0;
}

std::optional<std::vector<std::string>> _parse_csv_row(const std::string &line, char delimiter) {
  std::vector<std::string> row;
  std::string field;
  bool in_quotes = false;

  for (char c : line) {
    if (c == '"') {
      in_quotes = !in_quotes;
    } else if (c == delimiter && !in_quotes) {
      row.push_back(field);
      field.clear();
    } else {
      field.push_back(c);
    }
  }

  row.push_back(field);
  return row;
}

std::optional<CSVReader> read_csv(const std::string &filename, char delimiter) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return std::nullopt;
  }

  CSVReader reader;
  std::string line;

  while (std::getline(file, line)) {
    auto row = _parse_csv_row(line, delimiter);
    if (row.has_value()) {
      reader.data.push_back(row.value());
    }
  }
  reader.filename = filename;
  reader.delimiter = delimiter;

  return reader;
}


int filter_csv(CSVReader &reader, const std::string &column_name, const std::function<bool(const std::string &)> &predicate) {
  int count = 0;
  auto _column_index = reader.get_column_index(column_name);
  std::vector<std::vector<std::string>> filtered_data = std::vector<std::vector<std::string>>(reader.data.size());

  if (!_column_index.has_value()) {
    return -1;
  }
  unsigned int column_index = _column_index.value();
  for (const auto &row : reader.data) {
    if (row.size() <= column_index) {
      return -1;
    }

    if (predicate(row[column_index])) {
      filtered_data[count] = row;
      count++;
    }
  }

  reader.data.swap(filtered_data);
  reader.data.resize(count);
  return 0;
}



