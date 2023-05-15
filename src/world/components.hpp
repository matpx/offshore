#pragma once

#include "../gfx/renderable.hpp"

namespace components {

struct Transform {
  vec3 translation = {0, 0, 0};
  quat rotation = glm::identity<quat>();

  mat4 world = glm::identity<mat4>();
  glm::dmat4 x;

  void update() {
    world = glm::translate(glm::identity<mat4>(), translation) * glm::toMat4(rotation);
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

}  // namespace components
