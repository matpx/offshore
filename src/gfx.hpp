#pragma once

#include <SDL2/SDL_video.h>
#include "world.hpp"

namespace gfx {

  void init();

  void begin_frame(world::Entity& camera);

  void draw_sphere();

  void end_frame();
  
  void swap();
  
  void finish();
  
}

