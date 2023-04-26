#include "world.hpp"

namespace world {

  Entity *entities = nullptr;

  EntityId create(const Entity& entity) {
    arrpush(entities, entity);

    return arrlen(entities) - 1;
  }

  Entity* get(EntityId id) {
    assert(entities[id].valid);
  
    return &entities[id];
  }

  void finish() {
    arrfree(entities);
  }

}

