#include <memory>

class IntGrid {
public:
  IntGrid(int w, int h)
      : width(w), height(h), data(std::make_unique<int[]>(w * h)) {}

  int &operator()(int x, int y) {
    return data[static_cast<size_t>(y) * width + x];
  }

  const int &operator()(int x, int y) const {
    return data[static_cast<size_t>(y) * width + x];
  }

  int width;
  int height;
  std::unique_ptr<int[]> data;
};