#include "window.hpp"

#include <cassert>

#include <SDL2/SDL.h>
#include "../core/log.hpp"
#include "../core/types.hpp"

namespace gfx::window {

static SDL_Window* window = nullptr;

static u32 window_width = 1200;
static u32 window_height = 800;

void init() {
  LOG_INFO("gfx::window::init()");

  assert(window == nullptr);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    FATAL("SDL_Init() failed");
  }

  SDL_version sdl_version;
  SDL_GetVersion(&sdl_version);

  LOG_DEBUG("SDL_version: %d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);

  window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height,
                            SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    FATAL("SDL_CreateWindow() failed");
  }

  if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0) {
    LOG_ERROR("SDL_SetRelativeMouseMode() failed");
  }
}

void finish() {
  SDL_DestroyWindow(window);

  SDL_Quit();

  LOG_INFO("gfx::window::init()");
}

SDL_Window* get_sdl_window() { return window; }

uvec2 get_width_height() { return {window_width, window_height}; }

}  // namespace gfx::window
