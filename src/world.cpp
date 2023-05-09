#include "world.hpp"

#include "log.hpp"
#include "utils.hpp"

namespace world {

EntityId main_camera = INVALID_ENTITY;

utils::Vector<Entity> entities;
size_t entity_count = 0;

EntityId create(const Entity& entity) {
   if(entities.size() <= entity_count) {
    entities.set_size(8 + entities.size() * 3/2);
    LOG_DEBUG("resize world: %ld", entities.size());
  }

  assert(entity.variant != Entity::Variant::INVALID);
  assert((size_t)entities.data() % alignof(Entity) == 0);

  entities[entity_count] = entity;

  return entity_count++;
}

Entity& get(EntityId id) {
  assert(entities[id].variant != Entity::Variant::INVALID);

  return entities[id];
}

void finish() {
  entities.clear();

  LOG_INFO("world::finish()");
}

}  // namespace world
