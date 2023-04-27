#pragma once

#include "types.hpp"

namespace world {

  using EntityId = u32;

  struct Transform {
    vec3 translation;
    quat rotation;
  };

  struct Camera {
    mat4 projection;
    
    Camera(float fov, float aspect, float near, float far)
      : projection(HMM_Perspective_RH_NO(fov, aspect, near, far)) {}
  };

  struct Entity {
    enum class Variant : u8 {
      INVALID = 0,
      Camera,
    };
    
    Transform transform;

    union {
      Camera camera;
    };

    Variant variant;
    bool valid = true;

    Entity(Camera component) : camera(component), variant(Variant::Camera) {}
  };

  EntityId create(const Entity&);

  Entity* get(EntityId);

  void finish();

}

