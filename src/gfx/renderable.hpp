#pragma once

#include <sokol/sokol_gfx.h>

#include "../core/types.hpp"

namespace gfx {

using index_t = u16;

struct Vertex {
    float positions[3];
    float normal[3];
};

struct Mesh {
  sg_bindings bindings;
  u32 base_element;
  u32 num_elements;
  AABB local_aabb;
};

struct Material {
  sg_pipeline pipeline;
};

}  // namespace gfx
