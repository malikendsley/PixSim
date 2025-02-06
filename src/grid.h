#pragma once

#include <tuple>
#include <vector>

// Alias for a tuple of references to a cell's data
using Cell = std::tuple<int &, float &>;

class Chunk {
public:
  Chunk(int w, int h)
      : width(w), height(h), ids(w * h, 0), velocities(w * h, 0.0f) {}

  Cell operator()(int x, int y) {
    int idx = y * width + x;
    return std::tie(ids[idx], velocities[idx]);
  }

  inline int &getId(int x, int y) { return ids[y * width + x]; }

  inline float &getVelocity(int x, int y) { return velocities[y * width + x]; }

  inline const int &getId(int x, int y) const { return ids[y * width + x]; }

  inline const float &getVelocity(int x, int y) const {
    return velocities[y * width + x];
  }

  // Move the contents from cell (srcX, srcY) to cell (dstX, dstY).
  inline void moveCell(int srcX, int srcY, int dstX, int dstY) {
    if (!checkBounds(srcX, srcY) || !checkBounds(dstX, dstY))
      throw; // TODO: ideally, prevent this from happening by design

    auto [srcId, srcVelocity] = (*this)(srcX, srcY);
    auto [dstId, dstVelocity] = (*this)(dstX, dstY);
    dstId = srcId;
    dstVelocity = srcVelocity;

    srcId = 0;
    srcVelocity = 0.0f;
  }

  Chunk &operator*=(int scalar) {
    for (auto &value : ids) {
      value *= scalar;
    }
    return *this;
  }

  bool checkBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
  }

  void clear() { ids.assign(width * height, 0); }

  const int width;
  const int height;

private:
  std::vector<int> ids;
  std::vector<float> velocities;
};
