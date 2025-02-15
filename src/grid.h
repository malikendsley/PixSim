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

  Grid &operator*=(int scalar) {
    for (auto &value : data) {
      value *= scalar;
    }
    return *this;
  }

  bool checkBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
  }

  void clear() { data.assign(width * height, 0); }

  const int width;
  const int height;
  std::vector<int> data;
};