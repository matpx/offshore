#include "world.hpp"
#include "alloc.hpp"
#include <cassert>
#include <string.h>

namespace world {

  constexpr u32 ENTITY_COUNT = 64;

  Entity *entities = nullptr;
  u32 count        = 0;

  void init() {
    constexpr size_t entites_size = ENTITY_COUNT * sizeof(Entity);
  
    entities = (Entity*) omalloc(entites_size);
    memset((void*)entities, 0, entites_size);
  }

  EntityId create(const Entity& entity) {
    assert(count < ENTITY_COUNT);
  
    entities[count] = entity;

    return count++;
  }

  Entity* get(EntityId id) {
    assert(id < ENTITY_COUNT);
    assert(entities[id].valid);
  
    return &entities[id];
  }

  void finish() {
    ofree(entities);
  }

}

