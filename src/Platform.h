#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <span>

class Platform
{
    public:
        Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
        ~Platform();

        void Update(void const* buffer, int pitch);
        bool ProcessInput(std::span<uint8_t, 16> keys);

    private:
        SDL_Window* window{};
        SDL_Renderer* renderer{};
        SDL_Texture* texture{};

};
