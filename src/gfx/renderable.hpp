#pragma once

#include <nvrhi/nvrhi.h>

#include "../core/geometry.hpp"
#include "vertex.hpp"

namespace gfx {

struct Mesh {
  nvrhi::BufferHandle vertex_buffer;
  nvrhi::BufferHandle index_buffer;
  u32 base_element;
  u32 num_elements;
  geometry::AABB local_aabb;
};

struct Material {
  nvrhi::GraphicsPipelineHandle pipeline;
  nvrhi::BufferHandle constant_buffer;
  nvrhi::BindingSetHandle binding_set;
};

}  // namespace gfx
