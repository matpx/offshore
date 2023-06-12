#pragma once

#include <SDL2/SDL_events.h>

extern "C" {

struct nk_context* get_nk_context(void);

int nk_sdl_handle_event(SDL_Window* sdl_window, SDL_Event* evt);

}
