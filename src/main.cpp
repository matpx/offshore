#include <SDL2/SDL.h>

#include "gfx.hpp"
#include "input.hpp"
#include "utils.hpp"
#include "world.hpp"

int main() {
    LOG_DEBUG("debug mode!");

    gfx::init();

    world::Entity cam = world::Entity(
        world::Transform{HMM_V3(0, 0, 5)},
        world::Camera(1000, 1000, 1.2, 0.1, 100.0));

    world::main_camera = world::create(cam);

    bool running = true;
    SDL_Event sdl_event;

    while (running) {
        while (SDL_PollEvent(&sdl_event)) {
            if (sdl_event.type == SDL_QUIT) {
                running = false;
            }
            if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }

            input::handle_sdl_event(sdl_event);
        }

        gfx::begin_frame(world::get(world::main_camera));
        gfx::draw_sphere();
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
