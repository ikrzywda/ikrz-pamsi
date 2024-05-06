#include "csv/include/reader.hpp"
#include "algs/bucketsort.hpp"
#include "algs/mergesort.hpp"
#include "algs/quicksort.hpp"

#include <algorithm>
#include <charconv>
#include <iostream>
#include <string_view>
#include <vector>



struct MovieRating {
  unsigned int id;
  std::string title;
  float rating;
};

template <> struct Sortable<MovieRating> {
  static float key(const MovieRating &rating) { return rating.rating; }
};

template <> struct Sortable<int> {
  static int key(const int &el) { return el; }
};

MovieRating parse_movie_rating(const CSVRow &row) {
  MovieRating rating;
  rating.id = std::stoi(row[0]);
  rating.title = row[1];
  rating.rating = std::stof(row[2]);
  return rating;
}

float key(const MovieRating &rating) { return rating.rating; }

std::optional<std::vector<MovieRating>>
prepare_data(const std::string &filename, char delimiter,
             unsigned int items_count, unsigned int shuffle_count) {
  auto _reader = read_csv(filename, delimiter);
  if (_reader == std::nullopt) {
    std::cerr << "Failed to read CSV file" << std::endl;
    return std::nullopt;
  }
  CSVReader reader = _reader.value();
  filter_csv(reader, "rating",
             [](const std::string &rating) { return rating.length() > 0; });

  std::vector<MovieRating> ratings =
      decode_to<MovieRating>(reader.data, parse_movie_rating);
  std::vector<MovieRating> prepared_data;
  for (unsigned int i = 0; i < items_count; i++) {
    prepared_data.push_back(ratings[i % ratings.size()]);
  }

  for (unsigned int i = 0; i < shuffle_count; i++) {
    unsigned int index1 = rand() % reader.data.size();
    unsigned int index2 = rand() % reader.data.size();
    std::swap(prepared_data[index1], prepared_data[index2]);
  }

  return prepared_data;
}

#include <chrono>

std::chrono::milliseconds sort_data(std::vector<MovieRating> &data, const std::string &sort_type) {
  auto start = std::chrono::high_resolution_clock::now();
  if (sort_type == "quicksort") {
    quicksort<MovieRating, float>(data, 0, data.size() - 1);
  } else if (sort_type == "mergesort") {
    data = merge_sort<MovieRating, float>(data);
  } else if (sort_type == "bucketsort") {
    data = bucketsort<MovieRating, float>(data, 10);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto sort_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  return sort_time_ms;
}

int main(int argc, char **argv) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <filename> <delimiter> <items_count> <shuffle_count> "
                 "<sort_type> <repeat_count>"
              << std::endl;
    return 1;
  }

  std::string filename = argv[1];
  int sort_key_pos = std::stoi(argv[2]);
  unsigned int items_count = std::stoi(argv[3]);
  std::string sort_type = argv[4];
  unsigned int shuffle_passes = 0;

  if (sort_type != "quicksort" && sort_type != "mergesort" &&
      sort_type != "bucketsort") {
    std::cerr << "Invalid sort type" << std::endl;
    return 1;
  }

  if (argc >= 6) {
    shuffle_passes = std::stoi(argv[5]);
  }
  if (argc >= 7) {
    srand(std::stoi(argv[6]));
  }


  auto _data = prepare_data(filename, ',', items_count, shuffle_passes);
  if (_data == std::nullopt) {
    return 1;
  }
  std::cout << "Data prepared" << std::endl;
  std::vector<MovieRating> data = _data.value();
  std::cout << "Data size: " << data.size() << std::endl;
  std::chrono::milliseconds sort_time = sort_data(data, sort_type);
  std::cout << "Sort time: " << sort_time.count() << "ms" << std::endl;

  std::fstream file("sorted.csv", std::ios::out);
  for (const auto &rating : data) {
    file << rating.id << "," << rating.title << "," << rating.rating << std::endl;
  }

  return 0;
}
