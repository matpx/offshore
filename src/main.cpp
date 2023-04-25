#include "gfx.hpp"
#include "world.hpp"
#include <SDL2/SDL.h>

int main() {
  SDL_Window* window = gfx::init();

  world::init();

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

    gfx::swap();
  }

  world::finish();
  gfx::finish();

  return 0;
}

