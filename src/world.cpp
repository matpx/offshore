#include "world.hpp"

#include "log.hpp"
#include "utils.hpp"

namespace world {

EntityId main_camera = INVALID_ENTITY;

utils::Vector<Entity> entities;

EntityId create(const Entity& entity) {
  assert(entity.variant != Entity::Variant::INVALID);
  assert((size_t)entities.data() % alignof(Entity) == 0);

  entities.push(entity);

  return entities.size() - 1;
}

Entity& get(EntityId id) {
  assert(entities[id].variant != Entity::Variant::INVALID);

  return entities[id];
}

const utils::Span<Entity> get_entities() {
  return entities;
}

void clear() {
  entities.clear();

  LOG_INFO("world::clear()");
}

}  // namespace world
