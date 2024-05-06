#pragma once

#include <vector>
#include <stack>
#include "sortable.hpp"



template <typename T, typename K>
int partition(std::vector<T> &vec, int left, int right) {
  int pivot_index = rand() % (right - left + 1) + left;
  T pivot = vec[pivot_index];
  std::swap(vec[pivot_index], vec[right]);;
  pivot_index = left;

  // std::cout << "Left: " << left << " Right: " << right << " Pivot: " << "\n";
  for (int i = left; i < right; ++i) {
    if (Sortable<T>::key(vec[i]) < Sortable<T>::key(pivot)) {
      std::swap(vec[i], vec[pivot_index]);
      pivot_index += 1;
    }
  }

  std::swap(vec[pivot_index], vec[right]);
  return pivot_index;
}

template <typename T, typename K>
void quicksort(std::vector<T> &vec, int left, int right) {
    std::stack<std::pair<int, int>> stack;
    stack.push(std::make_pair(left, right));

    while (!stack.empty()) {
        std::pair<int, int> current = stack.top();
        stack.pop();

        int left = current.first;
        int right = current.second;

        if (left > right || left < 0) {
            continue;
        }

        int pivot_index = partition<T, K>(vec, left, right);

        // Push subarrays to stack for further partitioning
        stack.push(std::make_pair(left, pivot_index - 1));
        stack.push(std::make_pair(pivot_index + 1, right));
    }
}
