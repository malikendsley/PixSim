#pragma once

#include <SDL.h>

class InputHandler
{

public:
    struct InputState
    {
        int mouseX = 0;
        int mouseY = 0;
        bool lmb = false;
        bool oneKey = false;
        bool twoKey = false;
        bool threeKey = false;
        bool fourKey = false;
        bool cKey = false;
        bool quit = false; // SDL_QUIT event
    };
    InputState getInputState();

private:
    SDL_Event event;
};