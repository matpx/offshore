#include <SDL2/SDL.h>

#include "gfx.hpp"
#include "input.hpp"
#include "shapes.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "player.hpp"

int main() {
  LOG_DEBUG("debug mode!");

  gfx::init();
  player::setup();

  bool running = true;
  SDL_Event sdl_event;

  while (running) {
    input::clear();

    while (SDL_PollEvent(&sdl_event)) {
      if (sdl_event.type == SDL_QUIT) {
        running = false;
      }
      if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
      }

      input::handle_sdl_event(sdl_event);
    }

    player::update();

    gfx::begin_frame(world::get(world::main_camera));
    gfx::draw_world();
    gfx::shapes::draw_sphere();
    gfx::end_frame();

    gfx::present();
  }

  world::clear();
  gfx::finish();

  allocator::debug_leak_check();

  return 0;
}
