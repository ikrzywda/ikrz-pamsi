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

  std::vector<MovieRating> ratings = decode_to<MovieRating>(reader->data, parse_movie_rating);
  auto sorted = bucketsort<MovieRating, float>(ratings, 10);


  return 0;
}
