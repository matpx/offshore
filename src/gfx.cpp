#include "gfx.hpp"
#include <SDL2/SDL.h>

#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE33
#include "sokol/sokol_gfx.h"

namespace gfx {

  SDL_Window* window    = nullptr;
  SDL_GLContext context = nullptr;

  SDL_Window* init() {
    assert(window == nullptr && context == nullptr);
  
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    sg_desc desc = {};
    sg_setup(desc);

    return window;
  }

  void swap() {
    SDL_GL_SwapWindow(window);
  }

  void finish() {
    sg_shutdown();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    assert(glGetError() == 0);
    assert(SDL_GetError()[0] == '\0');
    
    SDL_Quit();
  }

}

