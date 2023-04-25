#pragma once

#include <SDL2/SDL_video.h>

namespace gfx {

  SDL_Window* init();

  void begin_frame();

  void draw_sphere();

  void end_frame();
  
  void swap();
  
  void finish();

}

