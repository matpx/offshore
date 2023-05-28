#include "world.hpp"

#include "../core/container.hpp"
#include "../core/log.hpp"

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
  for (size_t i = 0; i < entities.size(); i++) {
    world::Entity& entity = entities[i];
    components::Transform& transform = entity.transform;

    if (transform.parent_id == INVALID_ENTITY) {
      transform.update();
    }
  }

  for (size_t i = 0; i < entities.size(); i++) {
    world::Entity& entity = entities[i];
    components::Transform& transform = entity.transform;

    if (transform.parent_id != INVALID_ENTITY) {
      const components::Transform& parent_transform = get(transform.parent_id).transform;
      transform.update_from_parent(parent_transform);
    }
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
