#pragma once

#include <SDL2/SDL_events.h>

#include "types.hpp"

namespace input {

enum class Action {
  LEFT,
  RIGHT,
  UP,
  DOWN,
  _LEN,
};

void handle_sdl_event(SDL_Event& window_event);

void begin();

void end();

bool key_is_pressed(SDL_Keycode key);
bool action_is_pressed(Action action);

bool key_just_pressed(SDL_Keycode key);
bool action_just_pressed(Action action);

vec2 last_mouse_motion();

}  // namespace input
