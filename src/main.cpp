#include <SDL.h>
#include <memory>
#include <iostream>
#include <vector>
#include <numeric>

constexpr size_t simScale = 5; // "pixels" per sand grain. This is handled as simulation then integer scaling.
const size_t windowHeight = 400;
const size_t windowWidth = 400;
const int simHeight = windowHeight / simScale;
const int simWidth = windowWidth / simScale;
const int approxTargetFPS = 142;

// Function declarations (TODO: Fix this later)
static void updateSandTexture(SDL_Texture &texture, std::unique_ptr<int[]> &sand, int scaling, int simWidth, int simHeight);
static void updateUI(SDL_Texture &texture, int scaling, long long frame);
static int simulateSand(std::unique_ptr<int[]> &sand);
static inline int idx(int x, int y, int width);

int main(int argc, char *argv[])
{
	SDL_InitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowHeight, windowWidth, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);

	auto sand = std::make_unique<int[]>(simHeight * simWidth);

	bool running = true;
	SDL_Event event;
	auto lastTime = SDL_GetPerformanceCounter();
	auto simTick = 0;

	// TODO: Struct this hoe
	auto mouseX = 0;
	auto mouseY = 0;
	auto lmbDown = false;

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				std::cout << "Quitting" << std::endl;
				running = false;
			}
		}

		// Handle mouse input
		auto click = SDL_GetMouseState(&mouseX, &mouseY);
		lmbDown = click & SDL_BUTTON(SDL_BUTTON_LEFT);
		if (lmbDown)
		{
			std::cout << "Mouse down at (" << mouseX << ", " << mouseY << ")" << std::endl;
			// spawn some sand at the mouse
			if (mouseX >= 0 && mouseX < windowWidth && mouseY >= 0 && mouseY < windowHeight)
			{
				sand[(mouseY / simScale) * simWidth + (mouseX / simScale)] = 1;
			}
		}

		// The beginnings of a rendering pipeline

		// Update Textures
		updateSandTexture(*screenTexture, sand, simScale, simWidth, simHeight);
		updateUI(*screenTexture, simScale, simTick);

		// Draw the texture
		SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		// Step the sim
		auto moved = simulateSand(sand);

		simTick++;

		// TODO: I will need to properly accumulate time up to this point for true framerate,
		// but to wait for 1 / targetFPS, I will get roughly the right time
		SDL_Delay(1000 / approxTargetFPS);
	}

	SDL_DestroyTexture(screenTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

static int simulateSand(std::unique_ptr<int[]> &sand)
{
	// TODO: This is just for fun, remove it.
	auto moved = 0;

	for (int y = simHeight - 1; y >= 0; y--)
	{
		for (int x = 0; x < simWidth; x++)
		{
			if (sand[y * simWidth + x] == 1)
			{
				// Sand collides with the bottom of the screen
				if (y == simHeight - 1)
					continue;

				// Sand tries to fall down
				if (sand[(y + 1) * simWidth + x] == 0)
				{
					sand[y * simWidth + x] = 0;
					sand[(y + 1) * simWidth + x] = 1;
					moved++;
				}
				else
				{
					// If the pixel below is not empty, pick a random direction and move the grain there, if possible
					int direction = rand() % 2;
					if (direction == 0)
					{
						// Move the grain to the left
						if (x > 0 && sand[(y + 1) * simWidth + (x - 1)] == 0)
						{
							sand[y * simWidth + x] = 0;
							sand[(y + 1) * simWidth + (x - 1)] = 1;
							moved++;
						}
					}
					else
					{
						// Move the grain to the right
						if (x < simWidth - 1 && sand[(y + 1) * simWidth + (x + 1)] == 0)
						{
							sand[y * simWidth + x] = 0;
							sand[(y + 1) * simWidth + (x + 1)] = 1;
							moved++;
						}
					}
				}
			}
		}
	}

	return moved;
}

static void updateSandTexture(SDL_Texture &texture, std::unique_ptr<int[]> &sand, int scaling, int simWidth, int simHeight)
{
	void *pixels;
	int pitch;

	// Create intermediate buffer for the colored pixels prior to upscaling
	auto intermediateBuffer = std::make_unique<int[]>(simWidth * simHeight);

	SDL_LockTexture(&texture, nullptr, &pixels, &pitch);

	uint32_t *texturePixels = static_cast<uint32_t *>(pixels); // Assuming SDL_PIXELFORMAT_RGBA8888

	for (int simY = 0; simY < simHeight; simY++)
	{
		for (int simX = 0; simX < simWidth; simX++)
		{
			uint32_t color = (sand[(simY * simWidth) + simX] == 1)
								 ? 0xFFC2B280  // Precomputed RGBA value for light brown
								 : 0x00000000; // Precomputed RGBA value for black

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

	for (int dy = 0; dy < scaling; dy++)
	{
		for (int dx = 0; dx < scaling; dx++)
		{
			pixel_data[(row * scaling + dy) * (simWidth * scaling) + (col * scaling + dx) + (dot * scaling)] = 0xFFFFFFFF;
		}
	}

	SDL_UnlockTexture(&texture);
}

static inline int idx(int x, int y, int width)
{
	return y * width + x;
}