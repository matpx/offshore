#pragma once

#include "types.hpp"

namespace world {

  using EntityId = i32;

  constexpr EntityId INVALID_ENTITY = -1;
  
  struct Transform {
    float3 translation = {0, 0, 0};
    float4 rotation    = {0, 0, 0, 1};

    float4x4 world = linalg::identity;

    void update() {
      world = mul(linalg::translation_matrix(translation), linalg::rotation_matrix(rotation));
    }
  };

  struct Camera {
    float4x4 projection;
    
    Camera(float fov, float aspect, float near, float far)
      : projection(linalg::perspective_matrix(fov, aspect, near, far) ) {}
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

    Entity(Transform transform, Camera component)
      : transform(transform), camera(component), variant(Variant::Camera) {}
  };

  EntityId create(const Entity&);

  Entity& get(EntityId);

  void finish();

  extern EntityId main_camera;

}

