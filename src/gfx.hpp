#pragma once

#include <SDL2/SDL_video.h>

namespace gfx {

  SDL_Window* init();
  
  void swap();
  
  void finish();

}

