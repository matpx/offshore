#pragma once

#include "../core/geometry.hpp"

#include <sokol/sokol_gfx.h>

#include "vertex.hpp"

namespace gfx {

struct Mesh {
  sg_bindings bindings;
  u32 base_element;
  u32 num_elements;
  geometry::AABB local_aabb;
};

struct Material {
  sg_pipeline pipeline;
};

}  // namespace gfx
