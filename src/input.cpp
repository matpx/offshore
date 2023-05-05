#include "input.hpp"

namespace input {

    bool pressed[(size_t) Actions::_LEN];

    void handle_sdl_event(const SDL_Event& window_event) {
        if(window_event.type == SDL_KEYDOWN) {
            if(window_event.key.keysym.sym == SDLK_w) pressed[(size_t)Actions::UP]    = true;
            if(window_event.key.keysym.sym == SDLK_s) pressed[(size_t)Actions::DOWN]  = true;
            if(window_event.key.keysym.sym == SDLK_a) pressed[(size_t)Actions::LEFT]  = true;
            if(window_event.key.keysym.sym == SDLK_d) pressed[(size_t)Actions::RIGHT] = true;
        } else if(window_event.type == SDL_KEYDOWN) {
            if(window_event.key.keysym.sym == SDLK_w) pressed[(size_t)Actions::UP]    = false;
            if(window_event.key.keysym.sym == SDLK_s) pressed[(size_t)Actions::DOWN]  = false;
            if(window_event.key.keysym.sym == SDLK_a) pressed[(size_t)Actions::LEFT]  = false;
            if(window_event.key.keysym.sym == SDLK_d) pressed[(size_t)Actions::RIGHT] = false;
        }
    }

    bool is_pressed(Actions actions) {
        return pressed[(size_t) actions];
    }

}
