#pragma once

#include <SDL2/SDL_events.h>
namespace gfx::ui {

void init();

void begin_input();

void handle_input(SDL_Event* sdl_event);

void finish_input();

void begin_pass();

void finish_pass();

void finish();

}  // namespace gfx::ui
