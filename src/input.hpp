#pragma once

#include <SDL2/SDL_events.h>

#include "types.hpp"

namespace input {

enum class Actions {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    _LEN,
};

void handle_sdl_event(const SDL_Event& window_event);

bool is_pressed(Actions actions);

}  // namespace input
