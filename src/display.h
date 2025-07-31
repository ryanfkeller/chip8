#pragma once

#include <SDL2/SDL.h>
#include <array>
#include "utilities.h"
#include "sdl_manager.h"

class Display {
public:
    Display(SDL_Manager& sdl_manager);

    void clear();
    void render();
    void set_pixel(int x, int y, bool on);
    bool check_pixel(int x, int y);

    const std::array<bool, Utils::PIXEL_WIDTH * Utils::PIXEL_HEIGHT>& get_buffer() const;

private:
    SDL_Renderer* renderer_;
    std::array<bool, Utils::PIXEL_WIDTH * Utils::PIXEL_HEIGHT> buffer_; // 0 or 1 per pixel

    void draw_buffer();
};