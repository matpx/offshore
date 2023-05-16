#include "player.hpp"

#include "../core/input.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems::player {

static world::EntityId player_id = world::INVALID_ENTITY;

void setup() {
  world::main_camera = world::create(world::Entity({}, components::Camera(1000, 1000, 1.5, 0.1, 100.0)));
  player_id = world::create(world::Entity({.translation = {0, 0, 5}}, components::Player{}));
}

void update(double delta_time) {
  constexpr float look_multiplier = 0.005f;
  constexpr float velocity_multiplier = 0.005f;

  world::Entity& player = world::get(player_id);
  world::Entity& camera = world::get(world::main_camera);

  // rotation

  static vec2 head_rotation = {};

  head_rotation.x += input::last_mouse_motion().x * look_multiplier;
  head_rotation.y += input::last_mouse_motion().y * look_multiplier;

  player.transform.rotation = glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) *
                              glm::angleAxis(head_rotation.y, vec3{-1, 0, 0});

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

  player.transform.translation += player.transform.rotation * velocity;

  // camera parent

  camera.transform = player.transform;
}

}  // namespace systems::player
