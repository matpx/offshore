#include "player.hpp"

#include <entt/entity/registry.hpp>

#include "../core/input.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems::player {

static entt::entity player_entity = entt::null;

void setup() {
  player_entity = world::registry->create();

  world::registry->emplace<components::Transform>(player_entity, components::Transform{.translation = {0, 0, 12}});
  world::registry->emplace<components::Player>(player_entity, components::Player{});

  world::main_camera = world::registry->create();

  world::registry->emplace<components::Transform>(world::main_camera, components::Transform{.parent_id = player_entity});
  world::registry->emplace<components::Camera>(world::main_camera, components::Camera(1000, 1000, 1.5, 0.1, 100.0));
}

void update(double delta_time) {
  constexpr float look_multiplier = 0.005f;
  constexpr float velocity_multiplier = 0.005f;

  components::Transform& player_transform = world::registry->get<components::Transform>(player_entity);
  components::Transform& camera_transform = world::registry->get<components::Transform>(world::main_camera);

  // rotation

  static vec2 head_rotation = {};

  head_rotation.x += input::last_mouse_motion().x * look_multiplier;
  head_rotation.y += input::last_mouse_motion().y * look_multiplier;

  player_transform.rotation =
      glm::slerp(player_transform.rotation,
                 glm::rotation(glm::normalize(player_transform.rotation * vec3(0, 1, 0)), glm::normalize(player_transform.translation)) *
                     player_transform.rotation,
                 0.2f);

  camera_transform.rotation = glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) * glm::angleAxis(head_rotation.y, vec3{-1, 0, 0});

  // translation

  vec3 velocity = {};

  if (input::is_pressed(input::Actions::UP)) {
    velocity.z -= velocity_multiplier * delta_time;
  } else if (input::is_pressed(input::Actions::DOWN)) {
    velocity.z += velocity_multiplier * delta_time;
  }

  if (input::is_pressed(input::Actions::LEFT)) {
    velocity.x -= velocity_multiplier * delta_time;
  } else if (input::is_pressed(input::Actions::RIGHT)) {
    velocity.x += velocity_multiplier * delta_time;
  }

  player_transform.translation += player_transform.rotation * glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) * velocity;
}

}  // namespace systems::player
