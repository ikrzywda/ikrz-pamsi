#pragma once

#include <tuple>
#include "quicksort.hpp"
#include "sortable.hpp"

#define BUCKET_SIZE 10


template <typename T, typename K>
std::tuple<T, T> get_min_max(std::vector<T> &vec) {
  T min = vec[0];
  T max = vec[0];
  for (auto el : vec) {
    if (Sortable<T>::key(el) < Sortable<T>::key(min)) {
      min = el;
    }
    if (Sortable<T>::key(el) > Sortable<T>::key(max)) {
      max = el;
    }
  }
  return std::make_tuple(min, max);
}


template <typename T, typename K>
std::vector<T> bucketsort(std::vector<T> &vec, unsigned int bucket_count) {
  std::vector<std::vector<T>> buckets = std::vector<std::vector<T>>(bucket_count + 1);
  auto [min, max] = get_min_max<T, K>(vec);
  K range = Sortable<T>::key(max) - Sortable<T>::key(min);

  for (auto el : vec) {
    unsigned int bucket_index = (Sortable<T>::key(el) - Sortable<T>::key(min)) / (range / bucket_count);
    buckets[bucket_index].push_back(el);
  }

  for (auto &bucket : buckets) {
    quicksort<T, K>(bucket, 0, bucket.size() - 1);
  }

  std::vector<T> sorted;
  for (auto &bucket : buckets) {
    sorted.insert(sorted.end(), bucket.begin(), bucket.end());
  }
  
  return sorted;
}
