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

  nk_font_atlas* atlas;
  nk_sdl_font_stash_begin(&atlas);
  nk_sdl_font_stash_end();
}

void begin_input() { nk_input_begin(nk_ctx); }

void handle_input(SDL_Event* sdl_event) { nk_sdl_handle_event(sdl_event); }

void finish_input() { nk_input_end(nk_ctx); }

void begin_pass() {
  nk_clear(nk_ctx);

if (nk_begin(nk_ctx, "Show", nk_rect(50, 50, 220, 220),
    NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
    /* fixed widget pixel width */
    nk_layout_row_static(nk_ctx, 30, 80, 1);
    if (nk_button_label(nk_ctx, "button")) {
        /* event handling */
    }

    /* custom widget pixel width */
    nk_layout_row_begin(nk_ctx, NK_STATIC, 30, 2);
    {
        nk_layout_row_push(nk_ctx, 50);
        nk_label(nk_ctx, "Volume:", NK_TEXT_LEFT);
        nk_layout_row_push(nk_ctx, 110);
    }
    nk_layout_row_end(nk_ctx);
}
  nk_end(nk_ctx);
}

void finish_pass() { nk_sdl_render(); }

void finish() {
  assert(nk_ctx != nullptr);

  nk_sdl_shutdown();

  LOG_INFO("gfx::ui::finish()");
}

}  // namespace gfx::ui
