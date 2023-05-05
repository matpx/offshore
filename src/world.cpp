#include "world.hpp"
#include "utils.hpp"

namespace world {

  EntityId main_camera = INVALID_ENTITY;

  utils::Vector<Entity> entities;

  EntityId create(const Entity& entity) {
    entities.push(entity);

    return entities.size() - 1;
  }

  Entity& get(EntityId id) {
    assert(id < (int)entities.size());
    assert(entities[id].variant != Entity::Variant::INVALID);
  
    return entities[id];
  }

  void finish() {
    entities.clear();

    LOG_INFO("world::finish()");
  }

}

