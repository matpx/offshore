#include "world.hpp"
#include "container.hpp"
#include "log.hpp"

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
    DEBUG("finish world");
    
    arrfree(entities);
  }

}

