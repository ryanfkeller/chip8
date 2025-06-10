#include "display.h"

#include "utilities.h"

#include <SDL2/SDL.h>
#include <stdexcept>
#include <format>

Display::Display() {
    sdl_init();
    clear();
}
Display::~Display() {
    sdl_cleanup();
}

void Display::clear() {
    buffer_.fill(0);
    SDL_SetRenderDrawColor(
        renderer_, 
        Utils::PIXEL_COLOR_OFF.r,
        Utils::PIXEL_COLOR_OFF.g,
        Utils::PIXEL_COLOR_OFF.b,
        Utils::PIXEL_COLOR_OFF.a);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
}

void Display::render() {
    draw_buffer();
    SDL_RenderPresent(renderer_);
}

void Display::set_pixel(int x, int y, bool on) {
    if (x < 0 || x >= Utils::PIXEL_WIDTH || y < 0 || y >= Utils::PIXEL_HEIGHT) 
        return;
    buffer_[y * Utils::PIXEL_WIDTH + x] = on ? 1 : 0;
}

bool Display::check_pixel(int x, int y) {
    if (x < 0 || x >= Utils::PIXEL_WIDTH || y < 0 || y >= Utils::PIXEL_HEIGHT) {
        throw std::runtime_error(std::format("Invalid pixel check (x:{:}, y:{:}", x, y));
    }
    return buffer_[y * Utils::PIXEL_WIDTH + x];
}

const std::array<bool, Utils::PIXEL_WIDTH * Utils::PIXEL_HEIGHT>& Display::get_buffer() const {
    return buffer_;
}

void Display::draw_buffer() {
    SDL_SetRenderDrawColor(
        renderer_, 
        Utils::PIXEL_COLOR_OFF.r,
        Utils::PIXEL_COLOR_OFF.g,
        Utils::PIXEL_COLOR_OFF.b,
        Utils::PIXEL_COLOR_OFF.a);
    SDL_RenderClear(renderer_);

    SDL_SetRenderDrawColor(
        renderer_, 
        Utils::PIXEL_COLOR_ON.r,
        Utils::PIXEL_COLOR_ON.g,
        Utils::PIXEL_COLOR_ON.b,
        Utils::PIXEL_COLOR_ON.a);

    for (int y = 0; y < Utils::PIXEL_HEIGHT; y++) {
        for (int x = 0; x < Utils::PIXEL_WIDTH; x++) {
            if (buffer_[y * Utils::PIXEL_WIDTH + x]) {
                SDL_Rect pixel = {
                    x * Utils::DISPLAY_SCALE,
                    y * Utils::DISPLAY_SCALE,
                    Utils::DISPLAY_SCALE,
                    Utils::DISPLAY_SCALE
                };
                SDL_RenderFillRect(renderer_, &pixel);
            }
        }
    } 
}

void Display::sdl_init() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL video");
    }

    window_ = SDL_CreateWindow(
        Utils::WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        Utils::WINDOW_WIDTH,
        Utils::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window_) {
        throw std::runtime_error("Failed to create SDL window");
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        throw std::runtime_error("Failed to create SDL renderer");
    }
}

void Display::sdl_cleanup() {

    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    SDL_Quit();
}