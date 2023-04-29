#include "world.hpp"
#include "container.hpp"
#include "log.hpp"

namespace world {

  EntityId main_camera = INVALID_ENTITY;

  Entity *entities = nullptr;

  EntityId create(const Entity& entity) {
    arrpush(entities, entity);

    return arrlen(entities) - 1;
  }

  Entity& get(EntityId id) {
    assert(id < arrlen(entities));
    assert(entities[id].variant != Entity::Variant::INVALID);
  
    return entities[id];
  }

  void finish() {
    arrfree(entities);

    INFO("world::finish()");
  }

}

