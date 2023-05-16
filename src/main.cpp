#include <SDL2/SDL.h>

#include "./gfx/gfx.hpp"
#include "./core/input.hpp"
#include "./gfx/shapes.hpp"
#include "./core/container.hpp"
#include "./world/world.hpp"
#include "./systems/player.hpp"
#include "systems/terrain.hpp"

int main() {
  LOG_DEBUG("debug mode!");

  gfx::init();
  systems::player::setup();
  systems::terrain::create();

  Uint64 current_time = SDL_GetPerformanceCounter();
  Uint64 last_time;
  double delta_time;

  bool running = true;
  SDL_Event sdl_event;

  while (running) {
    last_time = current_time;
    current_time = SDL_GetPerformanceCounter();

    delta_time = (double)((current_time - last_time)*1000 / (double)SDL_GetPerformanceFrequency() );

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

    systems::player::update(delta_time);

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
