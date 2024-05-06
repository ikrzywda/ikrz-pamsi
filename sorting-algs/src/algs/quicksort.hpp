#pragma once

#include <vector>
#include "sortable.hpp"

template <typename T, typename K>
int partition(std::vector<T> &vec, int left, int right) {
  const T pivot = vec.at(right);
  unsigned int pivot_index = left;

  for (int i = left; i < right; ++i) {
    if (Sortable<T>::key(vec.at(i)) <= Sortable<T>::key(pivot)) {
      T tmp = vec.at(pivot_index);
      vec[pivot_index] = vec[i];
      vec[i] = tmp;
      pivot_index += 1;
    }
  }

  T tmp = vec[pivot_index];
  vec[pivot_index] = vec[right];
  vec[right] = tmp;
  return pivot_index;
}

template <typename T, typename K>
void quicksort(std::vector<T> &vec, int left, int right) {
  if (left >= right || left < 0) {
    return;
  }
  int pivot_index = partition<T, K>(vec, left, right);
  quicksort<T, K>(vec, left, pivot_index - 1); 
  quicksort<T, K>(vec, pivot_index, right);
}



