#pragma once

#include "defines.hpp"

namespace world {

  using EntityId = i32;

  struct Entity {
    HMM_Vec3 translation;
  };

  void init();

  EntityId create(const Entity&);

  Entity* get(EntityId);

  void finish();

}

