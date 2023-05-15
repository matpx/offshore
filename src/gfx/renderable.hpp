#pragma once

#include <sokol/sokol_gfx.h>

#include "../core/types.hpp"

namespace gfx {

struct Mesh {
  sg_bindings bindings;
  u32 num_elements;
};

struct Material {
  sg_pipeline pipeline;
};

}  // namespace gfx
