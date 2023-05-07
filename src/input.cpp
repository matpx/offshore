#include "input.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

namespace input {

bool pressed[(size_t)Actions::_LEN];

vec2 mouse_motion = {};

void set_key_state(SDL_Keycode key, bool state) {
  switch (key) {
    case SDLK_w:
      pressed[(size_t)Actions::UP] = state;
      return;
    case SDLK_s:
      pressed[(size_t)Actions::DOWN] = state;
      return;
    case SDLK_a:
      pressed[(size_t)Actions::LEFT] = state;
      return;
    case SDLK_d:
      pressed[(size_t)Actions::RIGHT] = state;
      return;
  }
}

void handle_sdl_event(const SDL_Event& window_event) {
  if (window_event.type == SDL_KEYDOWN) {
    set_key_state(window_event.key.keysym.sym, true);
  } else if (window_event.type == SDL_KEYUP) {
    set_key_state(window_event.key.keysym.sym, false);
  } else if (window_event.type == SDL_MOUSEMOTION) {
    mouse_motion.X += window_event.motion.xrel;
    mouse_motion.Y += window_event.motion.yrel;
  }
}

void clear() {
  mouse_motion = {};
}

bool is_pressed(Actions actions) {
  return pressed[(size_t)actions];
}

vec2 last_mouse_motion() {
  return mouse_motion;
}

}  // namespace input
