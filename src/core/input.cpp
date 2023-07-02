#include "input.hpp"

#include <array>

#include "../gfx/ui.hpp"
#include "log.hpp"

namespace input {

static std::array<bool, (size_t)Actions::_LEN> pressed;
static std::array<bool, (size_t)Toggle::_LEN> toggles;

static vec2 mouse_motion = {};

void set_key_state(const SDL_Keycode key, const bool state) {
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
    case SDLK_F5:
      if (state == true) {
        toggles[(size_t)Toggle::NOCLIP] = !toggles[(size_t)Toggle::NOCLIP];
        LOG_DEBUG("toggle NOCLIP");
      }
      return;
  }
}

void handle_sdl_event(SDL_Event& window_event) {
  if (window_event.type == SDL_KEYDOWN) {
    set_key_state(window_event.key.keysym.sym, true);
  } else if (window_event.type == SDL_KEYUP) {
    set_key_state(window_event.key.keysym.sym, false);
  } else if (window_event.type == SDL_MOUSEMOTION) {
    mouse_motion.x += window_event.motion.xrel;
    mouse_motion.y += window_event.motion.yrel;
  }

  gfx::ui::handle_input(&window_event);
}

void begin() {
  mouse_motion = {};
  gfx::ui::begin_input();
}

void end() { gfx::ui::finish_input(); }

bool is_pressed(Actions action) { return pressed[(size_t)action]; }

bool is_toggled(Toggle toggle) { return toggles[(size_t)toggle]; }

vec2 last_mouse_motion() { return mouse_motion; }

}  // namespace input
