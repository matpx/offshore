#include "player.hpp"

#include <entt/entity/registry.hpp>

#include "../core/input.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems {

static entt::entity player_entity = entt::null;

void Player::setup() {
  player_entity = world::registry->create();

  world::registry->emplace<comp::Transform>(player_entity, comp::Transform{.translation = {0, 0, 10}});
  world::registry->emplace<comp::Player>(player_entity, comp::Player{});

  world::main_camera = world::registry->create();

  world::registry->emplace<comp::Transform>(world::main_camera, comp::Transform{.parent_id = player_entity});
  world::registry->emplace<comp::Camera>(world::main_camera, comp::Camera(1000, 1000, 1.5, 0.1, 1000.0));
}

void Player::update(double delta_time) {
  constexpr float look_multiplier = 0.005f;
  constexpr float velocity_multiplier = 0.01f;

  comp::Transform& player_transform = world::registry->get<comp::Transform>(player_entity);
  comp::Transform& camera_transform = world::registry->get<comp::Transform>(world::main_camera);

  // rotation

  static vec2 head_rotation = {};

  head_rotation.x += input::last_mouse_motion().x * look_multiplier;
  head_rotation.y += input::last_mouse_motion().y * look_multiplier;

  // player_transform.rotation = glm::slerp(player_transform.rotation,
  //                                        glm::rotation(glm::normalize(player_transform.rotation * vec3(0, 1, 0)),
  //                                                      glm::normalize(player_transform.translation)) *
  //                                            player_transform.rotation,
  //                                        0.2f);

  camera_transform.rotation =
      glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) * glm::angleAxis(head_rotation.y, vec3{-1, 0, 0});

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

  // player_transform.translation +=
  //    player_transform.rotation * glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) * velocity;

  player_transform.translation += camera_transform.rotation * velocity;
}

}  // namespace systems
