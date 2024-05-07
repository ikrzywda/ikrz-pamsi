#pragma once

// Wygenerowane z GitHub Copilot
template <typename T>
struct Sortable {
  static auto key(const T& object) -> decltype(object.key()) {
    return object.key();
  }
};
