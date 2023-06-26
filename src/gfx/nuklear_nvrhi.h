#pragma once

#include <nuklear/nuklear.h>
#include <SDL2/SDL.h>

NK_API struct nk_context *nk_sdl_init(SDL_Window *win);
NK_API int nk_sdl_handle_event(SDL_Event *evt);
NK_API void nk_sdl_render();
NK_API void nk_sdl_shutdown(void);
