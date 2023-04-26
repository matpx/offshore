#include "gfx.hpp"
#include "world.hpp"
#include "log.hpp"
#include "container.hpp"
#include <SDL2/SDL.h>

int main() {
  DEBUG("debug mode!");

  gfx::init();

  bool running = true;
  SDL_Event sdl_event;

  while(running) {
    while(SDL_PollEvent(&sdl_event)) {
      if(sdl_event.type == SDL_QUIT) {
        running = false;
      }
      if(sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
      }
    }

    gfx::begin_frame();
    gfx::shapes::draw_sphere();
    gfx::end_frame();

    gfx::swap();
  }

  world::finish();
  gfx::finish();

#ifndef NDEBUG
  stb_leakcheck_dumpmem();
#endif

  return 0;
}

