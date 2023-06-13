#pragma once

#include <SDL2/SDL_video.h>
#include "../core/types.hpp"

namespace gfx::window {

void init();

void finish();

SDL_Window* get_sdl_window();
uvec2 get_width_height();

}  // namespace gfx::window
