#pragma once

#include <sokol/sokol_gfx.h>

#include "types.hpp"

namespace gfx {

struct Mesh {
  sg_bindings bindings;
  u32 num_elements;
};

}  // namespace gfx
