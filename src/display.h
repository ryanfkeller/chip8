#pragma once

#include <SDL2/SDL.h>
#include <array>
#include "utilities.h"

class Display {
public:
    Display();
    ~Display();

    void clear();
    void render();
    void set_pixel(int x, int y, bool on);
    bool check_pixel(int x, int y);

    const std::array<bool, Utils::PIXEL_WIDTH * Utils::PIXEL_HEIGHT>& get_buffer() const;

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    std::array<bool, Utils::PIXEL_WIDTH * Utils::PIXEL_HEIGHT> buffer_; // 0 or 1 per pixel

    void draw_buffer();
    void sdl_init();
    void sdl_cleanup();
};