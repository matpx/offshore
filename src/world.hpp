#pragma once

#include "types.hpp"
#include <typeindex>

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
    bool valid = true;
    Transform transform;

    std::type_index variant;

    union {
      Camera camera;
    };

    Entity(auto component) : variant(typeid(component)), camera(component) {}
  };

  EntityId create(const Entity&);

  Entity* get(EntityId);

  void finish();

}

