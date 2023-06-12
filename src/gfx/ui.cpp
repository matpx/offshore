#include "ui.hpp"

#include <SDL2/SDL.h>
#include <nuklear/nuklear.h>
#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_nuklear.h>

#include "../core/log.hpp"
#include "gfx.hpp"
#include "sokol_impl.h"

namespace gfx::ui {

nk_context* ctx = nullptr;

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

  if (nk_begin(ctx, "Show", nk_rect(50, 50, 220, 220), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
    /* fixed widget pixel width */
    nk_layout_row_static(ctx, 30, 80, 1);
    if (nk_button_label(ctx, "button")) {
      /* event handling */
    }

    static double value = 0.0f;
    nk_property_double(ctx, "paste", 0.0, &value, 100.0, 0.1, 1.0f);

    static char buf[101] = {0};
    nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, buf, 100, nk_filter_default);

    /* custom widget pixel width */
    nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
    {
      nk_layout_row_push(ctx, 50);
      nk_label(ctx, "Volume:", NK_TEXT_LEFT);
      nk_layout_row_push(ctx, 110);
      // nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
    }
    nk_layout_row_end(ctx);
  }

  nk_end(ctx);
}

void finish_pass() { snk_render(gfx::get_width_height().x, gfx::get_width_height().y); }

void finish() {
  snk_shutdown();
  LOG_INFO("gfx::ui::finish()");
}

}  // namespace gfx::ui
