#include "world.hpp"

#include "utils.hpp"

namespace world {

EntityId main_camera = INVALID_ENTITY;

utils::Vector<Entity, true> entities;
size_t entity_count = 0;

EntityId create(const Entity& entity) {
   if(entities.size() <= entity_count) {
    entities.set_size(1 + entities.size() * 2);
  }
  assert((u64)entities._data % alignof(Entity) == 0);

  entities[entity_count] = entity;

  return entity_count++;
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

static_assert(allocator::base_alignment >= std::alignment_of<world::Entity>());

}  // namespace world
