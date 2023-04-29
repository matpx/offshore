#pragma once

#include "types.hpp"
#include <hmm/HandmadeMath.h>

namespace world {

  using EntityId = i32;

  constexpr EntityId INVALID_ENTITY = -1;
  
  struct Transform {
    vec3 translation = v3(0, 0, 0);
    quat rotation    = q (0, 0, 0, 1);

    mat4 world = m4();

    void update() {
      world = HMM_Translate(translation);
    }
  };

  struct Camera {
    mat4 projection;
    
    Camera(float fov, float aspect, float near, float far) 
      : projection(HMM_Perspective_RH_ZO(fov, aspect, near, far)) {}
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

    Variant variant = Variant::INVALID;

    Entity(Transform transform, Camera component)
      : transform(transform), camera(component), variant(Variant::Camera) {}
  };

  EntityId create(const Entity&);

  Entity& get(EntityId);

  void finish();

  extern EntityId main_camera;

}

