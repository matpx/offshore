#pragma once

#include <SDL2/SDL_video.h>
#include "shapes.hpp"

namespace gfx {

  void init();

  void begin_frame();

  void end_frame();
  
  void swap();
  
  void finish();
  
}

