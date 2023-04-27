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
    
    Variant variant = Variant::INVALID;
    bool valid      = true;
    
    Transform transform;

    union {
      Camera camera;
    };

    Entity(Camera component) : variant(Variant::Camera), camera(component) {}
  };

  EntityId create(const Entity&);

  Entity* get(EntityId);

  void finish();

}

