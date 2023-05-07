#include "world.hpp"

#include <cstddef>

#include "utils.hpp"

namespace world {

EntityId main_camera = INVALID_ENTITY;

utils::Vector<Entity> entities;

EntityId create(const Entity& entity) {
  entities.push(entity);

  return entities.size() - 1;
}

Entity& get(EntityId id) {
  assert(id < entities.size());
  assert(entities[id].variant != Entity::Variant::INVALID);

  return entities[id];
}

void finish() {
  entities.clear();

  LOG_INFO("world::finish()");
}

}  // namespace world

static_assert(std::alignment_of<std::max_align_t>() >= std::alignment_of<world::Entity>());
