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

enum class Toggle {
  NOCLIP,
  _LEN,
};

void handle_sdl_event(SDL_Event& window_event);

void begin();

void end();

bool is_pressed(Actions action);
bool is_toggled(Toggle toggle);

vec2 last_mouse_motion();

}  // namespace input
