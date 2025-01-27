#pragma once

#include <vector>

class Grid {
public:
  Grid(int w, int h) : width(w), height(h), data(std::vector<int>(w * h)) {}

  int &operator()(int x, int y) {
    return data[static_cast<size_t>(y) * width + x];
  }

  const int &operator()(int x, int y) const {
    return data[static_cast<size_t>(y) * width + x];
  }

  void clear() { data.assign(width * height, 0); }

  int width;
  int height;
  std::vector<int> data;
};