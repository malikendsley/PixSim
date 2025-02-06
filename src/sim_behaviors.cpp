#include "sim_behaviors.h"
#include "grid.h"
#include "util.h"
#include <iostream>

// Simulate sand falling in the IntGrid
void simulateFalling(int x, int y, Chunk &grid) {
  // Sand collides with the bottom of the screen
  if (y == grid.height - 1)
    return;

  // Sand tries to fall straight down
  if (grid.getId(x, y + 1) == 0) {
    grid.moveCell(x, y, x, y + 1);
  }
  // Sand can only move diagonally if the sides are empty
  else {
    int dir = fastRand() == 1 ? -1 : 1;

    // Check the chosen direction
    if (grid.checkBounds(x + dir, y + 1) && grid.getId(x + dir, y + 1) == 0 &&
        grid.getId(x + dir, y) == 0) {
      grid.moveCell(x, y, x + dir, y + 1);
    }
    // Check the opposite direction if the first fails
    else {
      dir = -dir;
      if (grid.checkBounds(x + dir, y + 1) && grid.getId(x + dir, y + 1) == 0 &&
          grid.getId(x + dir, y) == 0) {
        grid.moveCell(x, y, x + dir, y + 1);
      }
    }
  }
}

// finished buffer is a 1d array representing the same grid as the tile buffer
void simulateFlowing(int x, int y, Chunk &tileBuffer,
                     std::vector<int> &finishedBuffer) {
  std::cout << "simulating flowing" << std::endl;
  // TODO, not a fan of this
  auto width = tileBuffer.width;

  // Check if we can flow down
  if (tileBuffer.checkBounds(x, y + 1) && tileBuffer.getId(x, y + 1) == 0) {
    tileBuffer.moveCell(x, y, x, y + 1);
    // use the width and height fields on the tile buffer to index the finished
    // buffer

    finishedBuffer[x + (y + 1) * width] = 1;
    return;
  }

  int rand = (fastRand() % 2 == 0 ? -1 : 1);

  // Check if we can flow down-left/down-right
  if (tileBuffer.checkBounds(x + rand, y + 1) &&
      tileBuffer.getId(x + rand, y + 1) == 0) {
    tileBuffer.moveCell(x, y, x + rand, y + 1);
    finishedBuffer[x + rand + (y + 1) * width] = 1;
    return;
  }

  if (tileBuffer.checkBounds(x - rand, y + 1) &&
      tileBuffer.getId(x - rand, y + 1) == 0) {
    tileBuffer.moveCell(x, y, x - rand, y + 1);
    finishedBuffer[x - rand + (y + 1) * width] = 1;
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
           tileBuffer.getId(x + rand * check, y) == 0) {
      check++;
    }

    // If we moved at least one cell
    if (check > 1) {
      int finalX = x + rand * (check - 1);
      // Double-check finalX is still in bounds
      if (tileBuffer.checkBounds(finalX, y)) {
        tileBuffer.moveCell(x, y, finalX, y);
        finishedBuffer[finalX + y * width] = 1;
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
           tileBuffer.getId(x + rand * check, y) == 0) {
      check++;
    }

    if (check > 1) {
      int finalX = x + rand * (check - 1);
      if (tileBuffer.checkBounds(finalX, y)) {
        tileBuffer.moveCell(x, y, finalX, y);
        finishedBuffer[finalX + y * width] = 1;
      }
      return;
    }
  }

  // If we can't move down or sideways, do nothing
  return;
}
