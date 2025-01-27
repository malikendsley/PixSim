#include <memory>

#include "grid.h"
#include "sim_behaviors.h"

// Simulate sand falling in the IntGrid
void simulateFalling(int x, int y, Grid &grid) {
  // Sand collides with the bottom of the screen
  if (y == grid.height - 1)
    return;

  // Sand tries to fall straight down
  if (grid(x, y + 1) == 0) {
    grid(x, y) = 0;
    grid(x, y + 1) = 1;
  }
  // Sand can only move diagonally if the sides are empty
  else {
    int dir =
        rand() % 2 == 0 ? -1 : 1; // Randomly choose left (-1) or right (+1)

    // Check the chosen direction
    if (x + dir >= 0 && x + dir < grid.width && grid(x + dir, y + 1) == 0 &&
        grid(x + dir, y) == 0) {
      grid(x, y) = 0;
      grid(x + dir, y + 1) = 1;
    }
    // Check the opposite direction if the first fails
    else {
      dir = -dir;
      if (x + dir >= 0 && x + dir < grid.width && grid(x + dir, y + 1) == 0 &&
          grid(x + dir, y) == 0) {
        grid(x, y) = 0;
        grid(x + dir, y + 1) = 1;
      }
    }
  }
}

// Flowing can be handled in the same pass as every other particle
// Assumes the coordinates contain flowable particle
void simulateFlowing(int x, int y, Grid &tileBuffer, Grid &flowBuffer,
                     Grid &finishedBuffer) {
  int tileType = tileBuffer(x, y);

  // Below and x+-1
  if (y + 1 < tileBuffer.height && tileBuffer(x, y + 1) == 0) {
    tileBuffer(x, y) = 0;
    tileBuffer(x, y + 1) = tileType;
    return;
  }
  // Randomly decide the diagonal direction (-1 or 1)
  int rand = std::rand() % 2 == 0 ? -1 : 1;

  // Check diagonal (down-left or down-right)
  if (y + 1 < tileBuffer.height && x + rand >= 0 &&
      x + rand < tileBuffer.width && tileBuffer(x + rand, y + 1) == 0) {
    tileBuffer(x, y) = 0;
    tileBuffer(x + rand, y + 1) = tileType;
    return;
  }

  // Check the opposite diagonal
  if (y + 1 < tileBuffer.height && x - rand >= 0 &&
      x - rand < tileBuffer.width && tileBuffer(x - rand, y + 1) == 0) {
    tileBuffer(x, y) = 0;
    tileBuffer(x - rand, y + 1) = tileType;
    return;
  }

  // If we can't flow down, try to move sideways
  rand = std::rand() % 2 == 0 ? -1 : 1;
  if (x + rand >= 0 && x + rand < tileBuffer.width &&
      tileBuffer(x + rand, y) == 0) {
    tileBuffer(x, y) = 0;
    tileBuffer(x + rand, y) = tileType;
    return;
  }
}
