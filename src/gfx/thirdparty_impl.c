#include <SDL2/SDL.h>

#define NK_IMPLEMENTATION
#include <nuklear/nuklear.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/util/sokol_nuklear.h>
#include <sokol/util/sokol_shape.h>

#define MC_IMPLEMENTATION
#include <rjm/rjm_mc.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

static void nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit* edit) {
  const char* text = SDL_GetClipboardText();
  if (text) nk_textedit_paste(edit, text, nk_strlen(text));
  (void)usr;
}

static void nk_sdl_clipboard_copy(nk_handle usr, const char* text, int len) {
  char* str = 0;
  (void)usr;
  if (!len) return;
  str = (char*)malloc((size_t)len + 1);
  if (!str) return;
  memcpy(str, text, (size_t)len);
  str[len] = '\0';
  SDL_SetClipboardText(str);
  free(str);
}

NK_API int nk_sdl_handle_event(SDL_Window* sdl_window, SDL_Event* evt) {
  struct nk_context* ctx = &_snuklear.ctx;

  /* optional grabbing behavior */
  if (ctx->input.mouse.grab) {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    ctx->input.mouse.grab = 0;
  } else if (ctx->input.mouse.ungrab) {
    int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_WarpMouseInWindow(sdl_window, x, y);
    ctx->input.mouse.ungrab = 0;
  }

  switch (evt->type) {
    case SDL_KEYUP: /* KEYUP & KEYDOWN share same routine */
    case SDL_KEYDOWN: {
      int down = evt->type == SDL_KEYDOWN;
      const Uint8* state = SDL_GetKeyboardState(0);
      switch (evt->key.keysym.sym) {
        case SDLK_RSHIFT: /* RSHIFT & LSHIFT share same routine */
        case SDLK_LSHIFT:
          nk_input_key(ctx, NK_KEY_SHIFT, down);
          break;
        case SDLK_DELETE:
          nk_input_key(ctx, NK_KEY_DEL, down);
          break;
        case SDLK_RETURN:
          nk_input_key(ctx, NK_KEY_ENTER, down);
          break;
        case SDLK_TAB:
          nk_input_key(ctx, NK_KEY_TAB, down);
          break;
        case SDLK_BACKSPACE:
          nk_input_key(ctx, NK_KEY_BACKSPACE, down);
          break;
        case SDLK_HOME:
          nk_input_key(ctx, NK_KEY_TEXT_START, down);
          nk_input_key(ctx, NK_KEY_SCROLL_START, down);
          break;
        case SDLK_END:
          nk_input_key(ctx, NK_KEY_TEXT_END, down);
          nk_input_key(ctx, NK_KEY_SCROLL_END, down);
          break;
        case SDLK_PAGEDOWN:
          nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
          break;
        case SDLK_PAGEUP:
          nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
          break;
        case SDLK_z:
          nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_r:
          nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_c:
          nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_v:
          nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_x:
          nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_b:
          nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_e:
          nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_UP:
          nk_input_key(ctx, NK_KEY_UP, down);
          break;
        case SDLK_DOWN:
          nk_input_key(ctx, NK_KEY_DOWN, down);
          break;
        case SDLK_LEFT:
          if (state[SDL_SCANCODE_LCTRL])
            nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
          else
            nk_input_key(ctx, NK_KEY_LEFT, down);
          break;
        case SDLK_RIGHT:
          if (state[SDL_SCANCODE_LCTRL])
            nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
          else
            nk_input_key(ctx, NK_KEY_RIGHT, down);
          break;
      }
    }
      return 1;

    case SDL_MOUSEBUTTONUP: /* MOUSEBUTTONUP & MOUSEBUTTONDOWN share same routine */
    case SDL_MOUSEBUTTONDOWN: {
      int down = evt->type == SDL_MOUSEBUTTONDOWN;
      const int x = evt->button.x, y = evt->button.y;
      switch (evt->button.button) {
        case SDL_BUTTON_LEFT:
          if (evt->button.clicks > 1) nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
          nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
          break;
        case SDL_BUTTON_MIDDLE:
          nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
          break;
        case SDL_BUTTON_RIGHT:
          nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
          break;
      }
    }
      return 1;

    case SDL_MOUSEMOTION:
      if (ctx->input.mouse.grabbed) {
        int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
        nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
      } else
        nk_input_motion(ctx, evt->motion.x, evt->motion.y);
      return 1;

    case SDL_TEXTINPUT: {
      nk_glyph glyph;
      memcpy(glyph, evt->text.text, NK_UTF_SIZE);
      nk_input_glyph(ctx, glyph);
    }
      return 1;

    case SDL_MOUSEWHEEL:
      nk_input_scroll(ctx, nk_vec2((float)evt->wheel.x, (float)evt->wheel.y));
      return 1;
  }
  return 0;
}

struct nk_context* get_nk_context(void) {
  _snuklear.ctx.clip.copy = nk_sdl_clipboard_copy;
  _snuklear.ctx.clip.paste = nk_sdl_clipboard_paste;

  return &_snuklear.ctx;
}
