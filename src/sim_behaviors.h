#pragma once

#include <SDL.h>

#include "grid.h"

// Function declarations (TODO: Fix this later)

void simulateFalling(int x, int y, Chunk &grid);
void simulateFlowing(int x, int y, Chunk &tileBuffer, std::vector<int> &finishedBuffer);
