#include <vector>

template <typename T>
int partition(std::vector<T> &vec, int left, int right) {
  const T pivot = vec.at(right);
  unsigned int pivot_index = left;

  for (int i = left; i < right; ++i) {
    if (vec.at(i) <= pivot) {
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

template <typename T>
void quicksort(std::vector<T> &vec, int left, int right) {
  if (left >= right || left < 0) {
    return;
  }
  int pivot_index = partition<T>(vec, left, right);

  quicksort<T>(vec, left, pivot_index - 1); 
  quicksort<T>(vec, pivot_index, right);
}



