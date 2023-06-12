#include "ui.hpp"

#include <SDL2/SDL.h>
#include <nuklear/nuklear.h>
#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_nuklear.h>

#include "../core/log.hpp"
#include "gfx.hpp"
#include "sokol_impl.h"

namespace gfx::ui {

static nk_context* ctx = nullptr;

void init() {
  LOG_INFO("gfx::ui::init()");

  snk_setup(snk_desc_t{});

  ctx = get_nk_context();
}

void begin_input() { nk_input_begin(ctx); }

void handle_input(SDL_Event* sdl_event) { nk_sdl_handle_event(gfx::get_sdl_window(), sdl_event); }

void finish_input() { nk_input_end(ctx); }

void begin_pass() {
  nk_clear(ctx);
}

void finish_pass() { snk_render(gfx::get_width_height().x, gfx::get_width_height().y); }

void finish() {
  snk_shutdown();
  LOG_INFO("gfx::ui::finish()");
}

}  // namespace gfx::ui
