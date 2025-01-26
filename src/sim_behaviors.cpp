#include <memory>

#include "grid.h"
#include "sim_behaviors.h"

// Right now sand's only behavior is to fall, but later on, we will have
// different types of particles
void simulateFalling(int x, int y, std::unique_ptr<int[]> &grid, int width,
                     int height) {
  // Sand collides with the bottom of the screen
  if (y == height - 1)
    return;
  // Sand tries to fall down
  if (grid[(y + 1) * width + x] == 0) {
    grid[y * width + x] = 0;
    grid[(y + 1) * width + x] = 1;
  }
  // we can only move diagonally if the sides are empty (can't clip through
  // 1-thick walls)
  else {
    // TODO: Extract this to one function
    int dir = rand() % 2 == 0 ? -1 : 1;
    // Check if the chosen direction is empty
    if (x + dir >= 0 && x + dir < width &&
        grid[(y + 1) * width + x + dir] == 0 &&
        grid[y * width + x + dir] == 0) {
      grid[y * width + x] = 0;
      grid[(y + 1) * width + x + dir] = 1;
    } else {
      dir = -dir;
      if (x + dir >= 0 && x + dir < width &&
          grid[(y + 1) * width + x + dir] == 0 &&
          grid[y * width + x + dir] == 0) {
        grid[y * width + x] = 0;
        grid[(y + 1) * width + x + dir] = 1;
      }
    }
  }
}

// Flowing can be handled in the same pass as every other particle
// Assumes the coordinates contain flowable particle
void simulateFlowing(int x, int y, std::unique_ptr<int[]> &tileBuffer,
                     std::unique_ptr<int[]> &flowBuffer,
                     std::unique_ptr<int[]> &finishedBuffer, int width,
                     int height) {
  int tileType = tileBuffer[y * width + x];

  // Check below, below left, and below right
  // Don't go outside of edge of the screen
  if (y + 1 < height && tileBuffer[(y + 1) * width + x] == 0) {
    tileBuffer[y * width + x] = 0;
    tileBuffer[(y + 1) * width + x] = tileType;
    return;
  }
  int rand = std::rand() % 2 == 0 ? -1 : 1;

  if (y + 1 < height && x + rand >= 0 && x + rand < width &&
      tileBuffer[(y + 1) * width + x + rand] == 0) {
    tileBuffer[y * width + x] = 0;
    tileBuffer[(y + 1) * width + x + rand] = tileType;
    return;
  }

  if (y + 1 < height && x - rand >= 0 && x - rand < width &&
      tileBuffer[(y + 1) * width + x - rand] == 0) {
    tileBuffer[y * width + x] = 0;
    tileBuffer[(y + 1) * width + x - rand] = tileType;
    return;
  }

  // If we can't flow down, we have to flow sideways
  rand = std::rand() % 2 == 0 ? -1 : 1;
  if (x + rand >= 0 && x + rand < width &&
      tileBuffer[y * width + x + rand] == 0) {
    tileBuffer[y * width + x] = 0;
    tileBuffer[y * width + x + rand] = tileType;
    return;
  }
}
