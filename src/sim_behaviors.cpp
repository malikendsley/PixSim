#include <memory>

#include "grid.h"
#include "sim_behaviors.h"
#include "util.h"

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
    int dir = fastRand() == 1 ? -1 : 1;

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

void simulateFlowing(int x, int y, Grid &tileBuffer, Grid &finishedBuffer) {
  int tileType = tileBuffer(x, y);

  // Check if we can flow down
  if (tileBuffer.checkBounds(x, y + 1) && tileBuffer(x, y + 1) == 0) {
    tileBuffer(x, y) = 0;
    tileBuffer(x, y + 1) = tileType;
    finishedBuffer(x, y + 1) = 1;
    return;
  }

  int rand = (fastRand() % 2 == 0 ? -1 : 1);

  // Check if we can flow down-left/down-right
  if (tileBuffer.checkBounds(x + rand, y + 1) &&
      tileBuffer(x + rand, y + 1) == 0) {
    tileBuffer(x, y) = 0;
    tileBuffer(x + rand, y + 1) = tileType;
    finishedBuffer(x + rand, y + 1) = 1;
    return;
  }

  if (tileBuffer.checkBounds(x - rand, y + 1) &&
      tileBuffer(x - rand, y + 1) == 0) {
    tileBuffer(x, y) = 0;
    tileBuffer(x - rand, y + 1) = tileType;
    finishedBuffer(x - rand, y + 1) = 1;
    return;
  }

  //
  // First sideways pass
  //
  rand = (fastRand() % 2 == 0 ? -1 : 1); // randomly choose a direction
  {
    int check = 1;
    // Move sideways until hitting an obstacle or going OOB
    while (tileBuffer.checkBounds(x + rand * check, y) &&
           tileBuffer(x + rand * check, y) == 0) {
      check++;
    }

    // If we moved at least one cell
    if (check > 1) {
      int finalX = x + rand * (check - 1);
      // Double-check finalX is still in bounds
      if (tileBuffer.checkBounds(finalX, y)) {
        tileBuffer(x, y) = 0;
        tileBuffer(finalX, y) = tileType;
        finishedBuffer(finalX, y) = 1;
      }
      return;
    }
  }

  //
  // Second sideways pass in the opposite direction
  //
  rand = -rand; // Reverse direction
  {
    int check = 1;
    while (tileBuffer.checkBounds(x + rand * check, y) &&
           tileBuffer(x + rand * check, y) == 0) {
      check++;
    }

    if (check > 1) {
      int finalX = x + rand * (check - 1);
      if (tileBuffer.checkBounds(finalX, y)) {
        tileBuffer(x, y) = 0;
        tileBuffer(finalX, y) = tileType;
        finishedBuffer(finalX, y) = 1;
      }
      return;
    }
  }

  // If we can't move down or sideways, do nothing
  return;
}
