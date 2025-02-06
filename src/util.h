#pragma once

#include "grid.h"

#include <SDL.h>
#include <random>

// Generate a random value inline
inline int fastRand() {
  static thread_local std::mt19937 generator;
  std::uniform_int_distribution<int> distribution(0, 1);
  return distribution(generator);
}

inline Chunk randomGrid(int width, int height, int value) {
  auto grid = Chunk(width, height);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Access the tuple returned by operator()
      const auto &cell = grid(x, y);

      // Set the first value in the tuple (ids[idx])
      std::get<0>(cell) = fastRand() == 1 ? 1 : 0;
    }
  }

  grid *= value;
  return grid;
}