#include "ui.hpp"

#include <SDL2/SDL.h>
#include <nuklear/nuklear.h>
// #include <sokol/sokol_gfx.h>
// #include <sokol/util/sokol_nuklear.h>

#include <cassert>

#include "../core/log.hpp"
#include "gfx.hpp"
#include "thirdparty_impl.h"
#include "window.hpp"

namespace gfx::ui {

static nk_context* nk_ctx = nullptr;

void init() {
  LOG_INFO("gfx::ui::init()");

  assert(nk_ctx == nullptr);

  // snk_setup(snk_desc_t{});

  // nk_ctx = get_nk_context();
}

void begin_input() { nk_input_begin(nk_ctx); }

void handle_input(SDL_Event* sdl_event) { 
  // nk_sdl_handle_event(window::get_sdl_window(), sdl_event); 
}

void finish_input() { nk_input_end(nk_ctx); }

void begin_pass() { nk_clear(nk_ctx); }

void finish_pass() { 
  // snk_render(window::get_width_height().x, window::get_width_height().y); 
}

void finish() {
  assert(nk_ctx != nullptr);

  // snk_shutdown();

  LOG_INFO("gfx::ui::finish()");
}

}  // namespace gfx::ui
