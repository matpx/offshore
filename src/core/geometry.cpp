#include "geometry.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace geometry {

AABB AABB::from_vertex_data(std::span<const gfx::Vertex> vertex_data) {
  vec3 min = vec3(std::numeric_limits<float>::infinity());
  vec3 max = -vec3(std::numeric_limits<float>::infinity());

  for (const gfx::Vertex& vertex : vertex_data) {  // TODO: not always needed
    if (vertex.positions[0] < min.x) min.x = vertex.positions[0];
    if (vertex.positions[1] < min.y) min.y = vertex.positions[1];
    if (vertex.positions[2] < min.z) min.z = vertex.positions[2];

    if (vertex.positions[0] > max.x) max.x = vertex.positions[0];
    if (vertex.positions[1] > max.y) max.y = vertex.positions[1];
    if (vertex.positions[2] > max.z) max.z = vertex.positions[2];
  }

  const vec3 center = (min + max) / 2.0f;

  return {
      .center = center,
      .extent = max - center,
  };
}

AABB AABB::transform_aabb(const mat4 transform_matrix) const {
  // https://zeux.io/2010/10/17/aabb-from-obb-with-component-wise-abs/

  mat4 rotation_mat = (mat3)transform_matrix;

  float* rotation_mat_ptr = glm::value_ptr(rotation_mat);

  for (i32 i = 0; i < 4 * 4; i++) {
    rotation_mat_ptr[i] = std::abs(rotation_mat_ptr[i]);
  }

  const vec3 new_center = transform_matrix * vec4(this->center, 1.0f);
  const vec3 new_extent = rotation_mat * vec4(this->extent, 1.0f);

  return {new_center, new_extent};
}

}  // namespace geometry