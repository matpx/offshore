#include "gfx.hpp"
#include "container.hpp"
#include "log.hpp"
#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_color.h>
#include <SDL2/SDL.h>

namespace gfx {

  SDL_Window* window    = nullptr;
  SDL_GLContext context = nullptr;

  SDL_Window* init() {
    INFO("init gfx");
    
    assert(window == nullptr && context == nullptr);

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    sg_desc desc = {};

#ifndef NDEBUG
    desc.allocator = {
      .alloc = [] (size_t size, [[maybe_unused]] void* user_data) { return stb_leakcheck_malloc(size, __FILE__, __LINE__); },
      .free =  [] (void* ptr, [[maybe_unused]] void* user_data)   { stb_leakcheck_free(ptr); },
    };
#endif
    
    sg_setup(desc);

    shapes::init();

    return window;
  }

  void begin_frame() {
    const sg_pass_action pass_action = {
      .colors = {{
          .action = SG_ACTION_CLEAR,
          .value = SG_BLACK,
        }},
    };

    sg_begin_default_pass(&pass_action, 1200, 800);
  }

  void end_frame() {
    sg_end_pass();
    sg_commit();
  }

  void swap() {
    SDL_GL_SwapWindow(window);
  }

  void finish() {
    shapes::finish();
  
    sg_shutdown();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    // assert(glGetError() == 0);
    assert(SDL_GetError()[0] == '\0');
    
    SDL_Quit();

    INFO("finished gfx");
  }

}

