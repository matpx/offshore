#include "ui.hpp"

#include <SDL2/SDL.h>
#include <nuklear/nuklear.h>

#include <cassert>

#include "../core/log.hpp"
#include "gfx.hpp"
#include "nuklear_nvrhi.h"
#include "window.hpp"

namespace gfx::ui {

static nk_context* nk_ctx = nullptr;

void init() {
  LOG_INFO("gfx::ui::init()");

  assert(nk_ctx == nullptr);

  nk_ctx = nk_sdl_init(window::get_sdl_window());
}

void begin_input() { nk_input_begin(nk_ctx); }

void handle_input(SDL_Event* sdl_event) { nk_sdl_handle_event(sdl_event); }

void finish_input() { nk_input_end(nk_ctx); }

void begin_pass() { nk_clear(nk_ctx); }

void finish_pass() { nk_sdl_render(); }

void finish() {
  assert(nk_ctx != nullptr);

  nk_sdl_shutdown();

  LOG_INFO("gfx::ui::finish()");
}

}  // namespace gfx::ui
