#include "input.hpp"

#include <SDL2/SDL_keycode.h>

#include <unordered_map>
#include <unordered_set>

#include "../gfx/ui.hpp"
#include "log.hpp"

namespace input {

static const std::unordered_map<SDL_Keycode, Action> key_to_action = {
    {SDLK_a, Action::LEFT},
    {SDLK_d, Action::RIGHT},
    {SDLK_w, Action::UP},
    {SDLK_s, Action::DOWN},
};

static std::unordered_set<SDL_Keycode> pressed_keys;
static std::unordered_set<SDL_Keycode> just_pressed_keys;

static std::unordered_set<Action> pressed_actions;
static std::unordered_set<Action> just_pressed_actions;

static vec2 mouse_motion = {};

void handle_key_event(const SDL_Keycode key, const bool down) {
  const auto action = key_to_action.find(key);

  if (down) {
    just_pressed_keys.emplace(key);
    pressed_keys.emplace(key);

    if (action != key_to_action.end()) {
      pressed_actions.emplace(action->second);
      just_pressed_actions.emplace(action->second);
    }
  } else {
    pressed_keys.erase(key);

    if (action != key_to_action.end()) {
      pressed_actions.erase(action->second);
    }
  }
}

void handle_sdl_event(SDL_Event& window_event) {
  if (window_event.type == SDL_KEYDOWN) {
    handle_key_event(window_event.key.keysym.sym, true);
  } else if (window_event.type == SDL_KEYUP) {
    handle_key_event(window_event.key.keysym.sym, false);
    pressed_keys.erase(window_event.key.keysym.sym);
  } else if (window_event.type == SDL_MOUSEMOTION) {
    mouse_motion.x += window_event.motion.xrel;
    mouse_motion.y += window_event.motion.yrel;
  }

  gfx::ui::handle_input(&window_event);
}

void begin() {
  mouse_motion = {};
  just_pressed_actions = {};
  just_pressed_keys = {};

  gfx::ui::begin_input();
}

void end() { gfx::ui::finish_input(); }

bool key_is_pressed(SDL_Keycode key) { return pressed_keys.contains(key); }
bool action_is_pressed(Action action) { return pressed_actions.contains(action); }

bool key_just_pressed(SDL_Keycode key) { return just_pressed_keys.contains(key); }
bool action_just_pressed(Action action) { return just_pressed_actions.contains(action); }

vec2 last_mouse_motion() { return mouse_motion; }

}  // namespace input
