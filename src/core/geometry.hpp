#pragma once

#include <span>

#include "../gfx/vertex.hpp"

namespace geometry {

struct AABB {
  vec3 center = vec3(0);
  vec3 extent = vec3(0);
};

AABB aabb_from_vertex_data(std::span<const gfx::Vertex> vertex_data);

AABB transform_aabb(const AABB& aabb, const mat4 transform_matrix);

}  // namespace geometry
