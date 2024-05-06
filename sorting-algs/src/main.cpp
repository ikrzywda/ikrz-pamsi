#include "csv/include/reader.hpp"

#include <iostream>
#include <charconv>
#include <string_view>
#include <vector>

#include "algs/quicksort.hpp"
#include "algs/mergesort.hpp"
#include "algs/bucketsort.hpp"

struct MovieRating {
  unsigned int id;
  std::string title;
  float rating;

};

template <>
struct Sortable<MovieRating> {
  static float key(const MovieRating &rating) {
    return rating.rating;
  }
};

template <>
struct Sortable<int> {
  static int key(const int &el) {
    return el;
  }
};


MovieRating parse_movie_rating(const CSVRow &row) {
  MovieRating rating;
  rating.id = std::stoi(row[0]);
  rating.title = row[1];
  rating.rating = std::stof(row[2]);
  return rating;
}


float key(const MovieRating &rating) {
  return rating.rating;
}



int main() {
  auto reader = read_csv("sample-data/projekt2_dane.csv");
  if (reader == std::nullopt) {
    std::cerr << "Failed to read CSV file" << std::endl;
    return 1;
  }
  filter_csv(reader.value(), "rating", [](const std::string &rating) {
    return rating.length() > 0;
  });
  std::cout << "Decoding data" << std::endl;
  std::vector<MovieRating> ratings = decode_to<MovieRating>(reader->data, parse_movie_rating);
  std::cout << "Sorting data" << std::endl;
  time_t start = time(0);
  quicksort<MovieRating, float>(ratings, 0, ratings.size() - 1);
  // merge_sort<MovieRating, float>(ratings);
  // auto sorted = bucketsort<MovieRating, float>(ratings, 10);
  // auto sorted = merge_sort<MovieRating, float>(ratings);


  time_t end = time(0);

  std::cout << "Quicksort took " << end - start << " seconds" << std::endl;

  std::fstream file("sorted.csv", std::ios::out);

  for (const auto &rating : ratings) {
    file << rating.id << "," << rating.title << "," << rating.rating << std::endl;
  }


  std::vector<int> dupa = {123,412,4,124,321,312,5,43,4,3,3};
  quicksort<int, int>(dupa, 0, dupa.size() - 1);
  for (const auto &el : dupa) {
    std::cout << el << std::endl;
  }

  // auto sorted = bucketsort<MovieRating, float>(ratings, 10);


  return 0;
}
