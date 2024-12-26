#include <SDL.h>

#include "input.h"

InputHandler::InputState InputHandler::getInputState()
{
    InputState input = {0, 0, false, false, false, false, false, false, false};

    // Process events
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            input.quit = true;
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
    auto click = SDL_GetMouseState(&input.mouseX, &input.mouseY);
    input.lmb = click & SDL_BUTTON(SDL_BUTTON_LEFT);

    return input;
}