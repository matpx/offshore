#include "world.hpp"
#include "alloc.hpp"
#include <cassert>

namespace world {

  constexpr size_t ENTITY_COUNT = 512;

  Entity *entities;
  size_t count = 0;

  void init() {
    entities = (Entity*) OMALLOC(ENTITY_COUNT * sizeof(Entity));
  }

  EntityId create(const Entity& entity) {
    assert(count < ENTITY_COUNT);
  
    entities[count] = entity;

    return count++;
  }

  Entity* get(EntityId id) {
    assert(id < ENTITY_COUNT);
  
    return entities;
  }

  void finish() {
    OFREE(entities);
  }

}

