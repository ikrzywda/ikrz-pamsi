#pragma once

#include "sortable.hpp"

#include <vector>
#include <numeric>
#include <iostream>

template <typename T, typename K>
std::vector<T> merge(std::vector<T> &vec_left, std::vector<T> &vec_right) {
    std::vector<T> sorted;
    unsigned int il = 0;
    unsigned int ir = 0;

    while (il < vec_left.size() && ir < vec_right.size()) {
        if (Sortable<T>::key(vec_left[il]) <= Sortable<T>::key(vec_right[ir])) {
            sorted.push_back(vec_left[il]);
            il++;
        } else {
            sorted.push_back(vec_right[ir]);
            ir++;
        }
    }
    while (il < vec_left.size()) {
        sorted.push_back(vec_left[il]);
        il++;
    }
    while (ir < vec_right.size()) {
        sorted.push_back(vec_right[ir]);
        ir++;
    }

    return sorted;
}

template <typename T, typename K>
std::vector<T> merge_sort(std::vector<T> &vec) {
    if (vec.size() <= 1) {
        return vec;
    }

    unsigned int half = vec.size() / 2;
    std::vector<T> left(vec.begin(), vec.begin() + half);
    std::vector<T> right(vec.begin() + half, vec.end());

    auto sorted_left = merge_sort(left);
    auto sorted_right = merge_sort(right);

    return merge(sorted_left, sorted_right);
}
