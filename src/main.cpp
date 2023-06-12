#include <SDL2/SDL.h>

#include "./core/input.hpp"
#include "./gfx/gfx.hpp"
#include "./gfx/shapes.hpp"
#include "./gfx/ui.hpp"
#include "core/log.hpp"
#include "systems/router.hpp"
#include "world/world.hpp"

int main() {
  LOG_DEBUG("debug mode!");

  gfx::init();
  world::init();
  systems::router::setup();

  Uint64 current_time = SDL_GetPerformanceCounter();
  Uint64 last_time;
  double delta_time;

  bool running = true;
  SDL_Event sdl_event;

  while (running) {
    last_time = current_time;
    current_time = SDL_GetPerformanceCounter();

    delta_time = (double)((current_time - last_time) * 1000 / (double)SDL_GetPerformanceFrequency());

    input::begin();

    while (SDL_PollEvent(&sdl_event)) {
      if (sdl_event.type == SDL_QUIT) {
        running = false;
      }
      if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
      }

      input::handle_sdl_event(sdl_event);
    }

    input::end();

    systems::router::frame(delta_time);
    world::update();

    gfx::begin_frame(world::main_camera);
    gfx::draw_world();

    gfx::shapes::begin_pass();
    // gfx::shapes::draw_sphere({2,0,0}, {3,1,1});
    // gfx::shapes::draw_box({0,1,0}, {1,3,1});
    gfx::shapes::finish_pass();

    gfx::ui::finish_pass();
    gfx::ui::begin_pass();

    gfx::end_frame();

    gfx::present();
  }

  gfx::finish();

  return 0;
}
