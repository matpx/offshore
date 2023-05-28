#include "player.hpp"

#include "../core/input.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems::player {

static EntityId player_id = INVALID_ENTITY;

void setup() {
  player_id = world::create(world::Entity({.translation = {0, 0, 12}}, components::Player{}));

  world::main_camera = world::create(world::Entity({.parent_id = player_id}, components::Camera(1000, 1000, 1.5, 0.1, 100.0)));
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

  player.transform.rotation = glm::slerp(player.transform.rotation, glm::rotation(glm::normalize(player.transform.rotation * vec3(0, 1, 0)), glm::normalize(player.transform.translation)) * player.transform.rotation, 0.2f);

  camera.transform.rotation = glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) *
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

  player.transform.translation += player.transform.rotation * glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) * velocity;
}

}  // namespace systems::player
