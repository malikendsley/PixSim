#pragma once

#include <SDL.h>
#include <memory>

// Function declarations (TODO: Fix this later)

void simulateFalling(int x, int y, std::unique_ptr<int[]> &sim, std::unique_ptr<int[]> &_, int width, int height);
void simulateFlowing(int x, int y, std::unique_ptr<int[]> &tileBuffer, std::unique_ptr<int[]> &flowBuffer, std::unique_ptr<int[]> &finishedBuffer, int width, int height);
