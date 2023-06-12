#pragma once

#include <entt/entity/entity.hpp>
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
};

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

  void update() {
    projection = glm::perspective(fov, (float)width / (float)height, near, far);
  }
};

struct Player {
};

struct Renderable {
  gfx::Mesh mesh;
  gfx::Material material;
};

}  // namespace comp
