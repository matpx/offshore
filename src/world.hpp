#pragma once

#include "types.hpp"

namespace world {

  using EntityId = u32;

  struct Entity {
    bool valid = true;
    
    vec3 translation;
    quat rotation;
  };

  EntityId create(const Entity&);

  Entity* get(EntityId);

  void finish();

}

