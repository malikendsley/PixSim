#include <SDL.h>
#include <memory>
#include <iostream>
#include <vector>
#include <numeric>
#include <SDL_mixer.h>
#include <functional>

#include "input.h"
#include "sim_behaviors.h"

constexpr size_t simScale = 1; // "pixels" per sim grain. This is handled as simulation then integer scaling.
constexpr size_t windowHeight = 800;
constexpr size_t windowWidth = 800;

// TODO: Eventually, we will parallelize on chunks, so make it parametric on height and width
constexpr int simHeight = windowHeight / simScale;
constexpr int simWidth = windowWidth / simScale;
constexpr int approxTargetFPS = 0;

static void draw(SDL_Texture &texture, std::unique_ptr<int[]> &sim, int scaling, int simWidth, int simHeight);
static void updateUI(SDL_Texture &texture, int scaling, long long frame);
static inline int idx(int x, int y, int width);

int main(int argc, char *argv[])
{

	SDL_InitSubSystem(SDL_INIT_EVERYTHING);
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

	SDL_Window *window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowHeight, windowWidth, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);

	InputHandler inputHandler;

	// Entries here correspond to what tile is at each position in the simulation grid
	auto tileTypeBuffer = std::make_unique<int[]>(simHeight * simWidth);
	// Entries here correspond to the flow of each tile in the simulation grid
	auto flowBuffer = std::make_unique<int[]>(simHeight * simWidth);
	// This buffer indicates which tiles have already been processed in the current frame
	auto processedBuffer = std::make_unique<int[]>(simHeight * simWidth);

	bool running = true;
	auto lastTime = SDL_GetPerformanceCounter();
	auto simTick = 0;

	enum simTile
	{
		air,
		sand,
		stone,
		water,
		gas,
	};
	simTile currentTile = sand;

	while (running)
	{
		auto input = inputHandler.getInputState();
		if (input.quit)
		{
			running = false;
		}
		if (input.lmb)
		{
			// spawn the held tile at the mouse position (currently just sand)
			if (input.mouseX >= 0 && input.mouseX < windowWidth && input.mouseY >= 0 && input.mouseY < windowHeight)
			{
				tileTypeBuffer[(input.mouseY / simScale) * simWidth + (input.mouseX / simScale)] = currentTile;
			}
		}

		// Handle keyboard input
		if (input.oneKey)
		{
			currentTile = sand;
			std::cout << "Current tile: sand" << std::endl;
		}
		if (input.twoKey)
		{
			currentTile = stone;
			std::cout << "Current tile: stone" << std::endl;
		}
		if (input.threeKey)
		{
			currentTile = water;
			std::cout << "Current tile: water" << std::endl;
		}
		if (input.fourKey)
		{
			currentTile = gas;
			std::cout << "Current tile: gas" << std::endl;
		}
		if (input.cKey)
		{
			std::fill(tileTypeBuffer.get(), tileTypeBuffer.get() + simHeight * simWidth, 0);
			std::cout << "Cleared the simulation" << std::endl;
		}

		// The beginnings of a rendering pipeline

		// Update Textures
		draw(*screenTexture, tileTypeBuffer, simScale, simWidth, simHeight);
		updateUI(*screenTexture, simScale, simTick);

		// Draw the texture
		SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		// Iterate bottom up on the simulation grid
		for (int y = simHeight - 1; y >= 0; y--)
		{
			for (int x = 0; x < simWidth; x++)
			{
				auto idx = y * simWidth + x;
				switch (tileTypeBuffer[idx])
				{
				case sand:
					simulateFalling(x, y, tileTypeBuffer, simWidth, simHeight);
					break;
				case water:
					simulateFlowing(x, y, tileTypeBuffer, flowBuffer, processedBuffer, simWidth, simHeight);
					break;
				case gas:
					break;
				}
			}
		}
		simTick++;
		// Reset the processed buffer
		std::fill_n(processedBuffer.get(), simHeight * simWidth, 0);

		// TODO: I will need to properly accumulate time up to this point for true framerate,
		// but to wait for 1 / targetFPS, I will get roughly the right time
		if constexpr (approxTargetFPS)
			SDL_Delay(1000 / approxTargetFPS);
	}

	SDL_DestroyTexture(screenTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

// Water is unique in that it has to flow and settle
// Volume preservation
static void volumePreservation(std::unique_ptr<int[]> &tileBuffer, std::unique_ptr<int[]> &flowBuffer, int width, int height, int tileType)
{
	// NOTE: i think that later, i will handle each body of water as a separate entity (with some way to altenatively handle small bits of water)
	//  For now, I will handle all water in the chunk in one pass

	// The flow buffer is the same size as the tile buffer, implicitly encoding the position of excess water and making the amount the value
	// For all water tiles, if flowBuffer[x, y] > 0, move the water to the nearest empty tile with a lower pressure (value in tileBuffer)
	// Prioritizing closest and downwards bias should give the most natural looking flow
	for (int y = 0; y < height; y++)
	{
		// x on the inner loop to improve cache locality
		for (int x = 0; x < width; x++)
		{
			if (tileBuffer[y * width + x] == tileType && flowBuffer[y * width + x] > 0)
			{
				// Find the nearest empty tile with a lower pressure
				// Start with the tile below
				if (y + 1 < height && tileBuffer[(y + 1) * width + x] == 0)
				{
					tileBuffer[y * width + x] = 0;
					tileBuffer[(y + 1) * width + x] = tileType;
					flowBuffer[y * width + x]--;
					flowBuffer[(y + 1) * width + x]++;
				}
				else
				{
					// If the tile below is occupied, check the sides
					if (x - 1 >= 0 && tileBuffer[y * width + x - 1] == 0)
					{
						tileBuffer[y * width + x] = 0;
						tileBuffer[y * width + x - 1] = tileType;
						flowBuffer[y * width + x]--;
						flowBuffer[y * width + x - 1]++;
					}
					else if (x + 1 < width && tileBuffer[y * width + x + 1] == 0)
					{
						tileBuffer[y * width + x] = 0;
						tileBuffer[y * width + x + 1] = tileType;
						flowBuffer[y * width + x]--;
						flowBuffer[y * width + x + 1]++;
					}
				}
			}
		}
	}
}

auto colors = std::vector<uint32_t>{
	0x00000000, // Air
	0xFFC26480, // Sand
	0xF0F0F080, // Stone
	0xA00FF00,	// Water
	0x00FF00FF, // Gas
};

static void draw(SDL_Texture &texture, std::unique_ptr<int[]> &grid, int scaling, int simWidth, int simHeight)
{
	void *pixels;
	int pitch;

	SDL_LockTexture(&texture, nullptr, &pixels, &pitch);

	uint32_t *texturePixels = static_cast<uint32_t *>(pixels); // Assuming SDL_PIXELFORMAT_RGBA8888

	if (scaling == 1)
	{
		// Special case: no scaling, directly write into the texture
		for (int simY = 0; simY < simHeight; simY++)
		{
			for (int simX = 0; simX < simWidth; simX++)
			{
				uint32_t color = colors[grid[simY * simWidth + simX]];
				texturePixels[simY * simWidth + simX] = color;
			}
		}
	}
	else
	{
		// General case: handle scaling
		for (int simY = 0; simY < simHeight; simY++)
		{
			for (int simX = 0; simX < simWidth; simX++)
			{
				uint32_t color = colors[grid[simY * simWidth + simX]];
				// Write directly into the texture, accounting for scaling
				for (int dy = 0; dy < scaling; dy++)
				{
					for (int dx = 0; dx < scaling; dx++)
					{
						texturePixels[(simY * scaling + dy) * (simWidth * scaling) + (simX * scaling + dx)] = color;
					}
				}
			}
		}
	}

	SDL_UnlockTexture(&texture);
}

static void updateUI(SDL_Texture &texture, int scaling, long long frame)
{
	void *pixels;
	int pitch;

	SDL_LockTexture(&texture, nullptr, &pixels, &pitch);

	Uint32 *pixel_data = (Uint32 *)pixels;

	auto row = 2;
	auto col = simWidth - 5;
	auto dot = frame % 3;
	if (scaling != 1)
	{
		for (int dy = 0; dy < scaling; dy++)
		{
			for (int dx = 0; dx < scaling; dx++)
			{
				pixel_data[(row * scaling + dy) * (simWidth * scaling) + (col * scaling + dx) + (dot * scaling)] = 0xFFFFFFFF;
			}
		}
	}
	else
	{
		// no need to scale, just loop normally
		pixel_data[row * simWidth + col + dot] = 0xFFFFFFFF;
	}

	SDL_UnlockTexture(&texture);
}

static inline int idx(int x, int y, int width)
{
	return y * width + x;
}