#pragma once

#include "components.hpp"
#include "../core/container.hpp"

namespace world {

using EntityId = u32;

constexpr EntityId INVALID_ENTITY = std::numeric_limits<u32>::max();

struct Entity {
  enum class Variant : u8 {
    INVALID = 0,
    Player,
    Camera,
    Renderable,
  };

  const char* name;
  components::Transform transform;

  union {
    components::Camera camera;
    components::Player player;
    components::Renderable renderable;
  };

  Variant variant;

  Entity(components::Transform transform, components::Camera component, const char* name = "camera")
      : name(name), transform(transform), camera(component), variant(Variant::Camera) {}

  Entity(components::Transform transform, components::Player component, const char* name = "player")
      : name(name), transform(transform), player(component), variant(Variant::Player) {}

  Entity(components::Transform transform, components::Renderable component, const char* name = "renderable")
      : name(name), transform(transform), renderable(component), variant(Variant::Renderable) {}

  constexpr components::Camera& get_camera() {
    assert(variant == Variant::Camera);
    return camera;
  }

  constexpr components::Player& get_player() {
    assert(variant == Variant::Player);
    return player;
  }
};

EntityId create(const Entity&);

Entity& get(EntityId);

const container::Span<Entity> get_entities();

void clear();

extern EntityId main_camera;

}  // namespace world
