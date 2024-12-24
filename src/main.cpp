#include <SDL.h>
#include <memory>
#include <iostream>
#include <vector>
#include <numeric>
#include <SDL_mixer.h>
#include <vector>
#include <functional>

constexpr size_t simScale = 10; // "pixels" per sim grain. This is handled as simulation then integer scaling.
const size_t windowHeight = 800;
const size_t windowWidth = 800;

// TODO: Eventually, we will parallelize on chunks, so make it parametric on height and width
const int simHeight = windowHeight / simScale;
const int simWidth = windowWidth / simScale;
const int approxTargetFPS = 142;

Mix_Chunk *sandSound = nullptr;

// Function declarations (TODO: Fix this later)
static void updateSandTexture(SDL_Texture &texture, std::unique_ptr<int[]> &sim, int scaling, int simWidth, int simHeight);
static void updateUI(SDL_Texture &texture, int scaling, long long frame);
static void simulateFalling(int x, int y, std::unique_ptr<int[]> &sim, int width = simWidth, int height = simHeight);
static inline int idx(int x, int y, int width);

int main(int argc, char *argv[])
{

	SDL_InitSubSystem(SDL_INIT_EVERYTHING);
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
	sandSound = Mix_LoadWAV("assets/sand.wav");
	if (sandSound == nullptr)
	{
		std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
	}

	SDL_Window *window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowHeight, windowWidth, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);

	std::vector<std::function<void(int, int, std::unique_ptr<int[]> &, int, int)>> passes = {
		[](int, int, std::unique_ptr<int[]> &, int, int) {}, // no-op pass
		simulateFalling,
	};

	auto sim = std::make_unique<int[]>(simHeight * simWidth);

	bool running = true;
	SDL_Event event;
	auto lastTime = SDL_GetPerformanceCounter();
	auto simTick = 0;

	// TODO: Struct this hoe
	auto mouseX = 0;
	auto mouseY = 0;

	struct keyDownState
	{
		bool lmb = false;
		bool oneKey = false;
		bool twoKey = false;
		bool threeKey = false;
		bool fourKey = false;
		bool cKey = false;
	};

	enum simTile
	{
		sand,
		stone,
		water,
		gas,
	};
	simTile currentTile = sand;

	while (running)
	{

		// Alternative is to declare this outside
		keyDownState input;
		// Process events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				std::cout << "Quitting" << std::endl;
				running = false;
			}
			// populate the struct with the current state of the keys
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					input.oneKey = true;
					break;
				case SDLK_2:
					input.twoKey = true;
					break;
				case SDLK_3:
					input.threeKey = true;
					break;
				case SDLK_4:
					input.fourKey = true;
					break;
				case SDLK_c:
					input.cKey = true;
					break;
				}
			}
		}

		// Handle mouse input
		auto click = SDL_GetMouseState(&mouseX, &mouseY);
		input.lmb = click & SDL_BUTTON(SDL_BUTTON_LEFT);
		if (input.lmb)
		{
			// spawn the held tile at the mouse position (currently just sand)
			if (mouseX >= 0 && mouseX < windowWidth && mouseY >= 0 && mouseY < windowHeight)
			{
				sim[(mouseY / simScale) * simWidth + (mouseX / simScale)] = 1;
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
			std::fill(sim.get(), sim.get() + simHeight * simWidth, 0);
			std::cout << "Cleared the simulation" << std::endl;
		}

		// The beginnings of a rendering pipeline

		// Update Textures
		updateSandTexture(*screenTexture, sim, simScale, simWidth, simHeight);
		updateUI(*screenTexture, simScale, simTick);

		// Draw the texture
		SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		// Iterate bottom up on the simulation grid
		for (int y = simHeight - 1; y >= 0; y--)
		{
			for (int x = 0; x < simWidth; x++)
			{
				passes[sim[y * simWidth + x]](x, y, sim, simWidth, simHeight);
			}
		}
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

// Right now sand's only behavior is to fall, but later on, we will have different types of particles
static void simulateFalling(int x, int y, std::unique_ptr<int[]> &grid, int width, int height)
{
	// TODO: Just for fun, remove later
	auto moved = false;

	// Sand collides with the bottom of the screen
	if (y == height - 1)
		return;
	// Sand tries to fall down
	if (grid[(y + 1) * width + x] == 0)
	{
		grid[y * width + x] = 0;
		grid[(y + 1) * width + x] = 1;
		moved = true;
	}
	else
	{
		// If the pixel below is not empty, pick a random direction and move the grain there, if possible
		int direction = rand() % 2;
		if (direction == 0)
		{
			// Move the grain to the left
			if (x > 0 && grid[(y + 1) * width + (x - 1)] == 0)
			{
				grid[y * width + x] = 0;
				grid[(y + 1) * width + (x - 1)] = 1;
				moved = true;
			}
		}
		else
		{
			// Move the grain to the right
			if (x < width - 1 && grid[(y + 1) * width + (x + 1)] == 0)
			{
				grid[y * width + x] = 0;
				grid[(y + 1) * width + (x + 1)] = 1;
				moved = true;
			}
		}
	}

	if (moved)
	{
		if (Mix_Playing(0) == 0)
		{
			Mix_PlayChannel(0, sandSound, -1);
		}
	}
	else
	{
		Mix_HaltChannel(0);
	}
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