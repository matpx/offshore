#pragma once

#include <cassert>

#include "types.hpp"

namespace world {

using EntityId = u32;

constexpr EntityId INVALID_ENTITY = std::numeric_limits<u32>::max();

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

struct Entity {
  enum class Variant : u8 {
    INVALID = 0,
    Player,
    Camera,
  };

  const char* name = "no_name";

  Transform transform;

  union {
    Camera camera;
    Player player;
  };

  Variant variant;

  Entity(Transform transform, Camera component, const char* name = "camera")
      : name(name), transform(transform), camera(component), variant(Variant::Camera) {}

  Entity(Transform transform, Player component, const char* name = "player")
      : name(name), transform(transform), player(component), variant(Variant::Player){}

  constexpr Camera& get_camera() {
    assert(variant == Variant::Camera);
    return camera;
  }

  constexpr Player& get_player() {
    assert(variant == Variant::Player);
    return player;
  }

};

EntityId create(const Entity&);

Entity& get(EntityId);

void finish();

extern EntityId main_camera;

}  // namespace world
