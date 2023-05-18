#include "world.hpp"

#include "../core/log.hpp"
#include "../core/container.hpp"

namespace world {

EntityId main_camera = INVALID_ENTITY;

static container::Vector<Entity> entities;

EntityId create(const Entity& entity) {
  assert(entity.variant != Entity::Variant::INVALID);
  assert((size_t)entities.data() % alignof(Entity) == 0);

  entities.push(entity);

  return entities.size() - 1;
}

void update() {
  container::Span<world::Entity> entities = world::get_entities();

  for (size_t i = 0; i < entities.size(); i++) {
    world::Entity& entity = entities[i];
   
    components::Transform& transform = entity.transform;
    transform.update();
  }
}

Entity& get(EntityId id) {
  assert(entities[id].variant != Entity::Variant::INVALID);

  return entities[id];
}

const container::Span<Entity> get_entities() {
  return entities;
}

void clear() {
  entities.clear();

  LOG_INFO("world::clear()");
}

}  // namespace world
