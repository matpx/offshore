#pragma once

#include <entt/entity/entity.hpp>
#include <limits>

#include "../gfx/renderable.hpp"

namespace comp {

struct Transform {
  vec3 translation = {0, 0, 0};
  quat rotation = glm::identity<quat>();
  mat4 world = glm::identity<mat4>();

  entt::entity parent_id = entt::null;

  void update() {
    assert(parent_id == entt::null);

    world = glm::translate(glm::identity<mat4>(), translation) * glm::toMat4(rotation);
  }

  void update_from_parent(const Transform& parent) {
    assert(parent_id != entt::null);

    world = glm::translate(glm::identity<mat4>(), translation) * glm::toMat4(rotation);
    world = parent.world * world;
  }

  inline AABB transform_aabb(const AABB& aabb) { // TODO move
    vec3 corners[8] = {
        aabb.min,
        {aabb.min.x, aabb.min.y, aabb.max.z},
        {aabb.min.x, aabb.max.y, aabb.min.z},
        {aabb.max.x, aabb.min.y, aabb.min.z},
        {aabb.min.x, aabb.max.y, aabb.max.z},
        {aabb.max.x, aabb.min.y, aabb.max.z},
        {aabb.max.x, aabb.max.y, aabb.min.z},
        aabb.max,
    };

    vec3 min = vec3(std::numeric_limits<float>::infinity());
    vec3 max = vec3(-std::numeric_limits<float>::infinity());

    for (vec3& corner : corners) {
      corner = world * vec4(corner, 1.0f);

      if (corner.x < min.x) min.x = corner.x;
      if (corner.y < min.y) min.y = corner.y;
      if (corner.z < min.z) min.z = corner.z;

      if (corner.x > max.x) max.x = corner.x;
      if (corner.y > max.y) max.y = corner.y;
      if (corner.z > max.z) max.z = corner.z;
    }

    return {
        min,
        max,
    };
  }
};  // namespace comp

struct Camera {
  u32 width;
  u32 height;
  float fov;
  float near;
  float far;

  mat4 projection;

  Camera(u32 width, u32 height, float fov, float near, float far)
      : width(width), height(height), fov(fov), near(near), far(far) {
    update();
  }

  void update() { projection = glm::perspective(fov, (float)width / (float)height, near, far); }
};

struct Player {};

struct Renderable {
  gfx::Mesh mesh;
  gfx::Material material;
};

}  // namespace comp
