#pragma once

#include "quicksort.hpp"

#define BUCKET_SIZE 10

template <typename T>
std::vector<T> bucketsort(std::vector<T> &vec, unsigned int bucket_count) {
  std::vector<std::vector<T>> buckets = std::vector<std::vector<T>>(bucket_count + 1);
  T max = *std::max_element(vec.begin(), vec.end());
  T min = *std::min_element(vec.begin(), vec.end());
  T range = max - min;

  for (auto el : vec) {
    unsigned int bucket_index = (el - min) / (range / bucket_count);
    buckets[bucket_index].push_back(el);
  }

  for (auto &bucket : buckets) {
    quicksort<T>(bucket, 0, bucket.size() - 1);
  }

  std::vector<T> sorted;
  for (auto &bucket : buckets) {
    sorted.insert(sorted.end(), bucket.begin(), bucket.end());
  }
  
  return sorted;
}
