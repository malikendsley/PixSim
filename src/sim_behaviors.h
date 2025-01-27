#pragma once

#include <SDL.h>

#include "grid.h"

// Function declarations (TODO: Fix this later)

void simulateFalling(int x, int y, Grid &grid);
void simulateFlowing(int x, int y, Grid &tileBuffer, Grid &flowBuffer,
                     Grid &finishedBuffer);
