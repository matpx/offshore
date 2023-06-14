#pragma once

#include <entt/entity/entity.hpp>
#include <glm/gtx/quaternion.hpp>
#include <limits>
#include <glm/gtc/type_ptr.hpp>

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

  inline AABB transform_aabb(const AABB& aabb) {  // TODO move
    mat4 rotation_mat = (mat3)world;

    float *rotation_mat_ptr = glm::value_ptr(rotation_mat);

    for (i32 i = 0; i < 4*4; i++) {
      rotation_mat_ptr[i] = std::abs(rotation_mat_ptr[i]);
    }

    const vec3 center = (aabb.min + aabb.max) / 2.0f;
    const vec3 extent = (aabb.max - aabb.min) / 2.0f;

    const vec3 new_center = world * vec4(center, 1.0f);
    const vec3 new_extent = rotation_mat * vec4(extent, 1.0f);

    return {new_center - new_extent, new_center + new_extent};
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
