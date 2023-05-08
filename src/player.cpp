#include "player.hpp"

#include "input.hpp"
#include "log.hpp"
#include "types.hpp"
#include "world.hpp"

namespace player {

static world::EntityId player_id = world::INVALID_ENTITY;

void setup() {
  world::main_camera = world::create(world::Entity({}, world::Camera(1000, 1000, 1.5, 0.1, 100.0)));
  player_id = world::create(world::Entity({.translation = {0, 0, 5}}, world::Player{}));
}

void update() {
  world::Entity& player = world::get(player_id);
  world::Entity& camera = world::get(world::main_camera);

  // rotation

  static vec2 head_rotation = {};

  head_rotation.x += input::last_mouse_motion().x * 0.001f;
  head_rotation.y += input::last_mouse_motion().y * 0.001f;

  player.transform.rotation = glm::angleAxis(head_rotation.x, vec3 {0, -1, 0}) *
                              glm::angleAxis(head_rotation.y, vec3 {-1, 0, 0});

  // translation

  vec3 velocity = {};

  if (input::is_pressed(input::Actions::UP)) {
    velocity.z -= 0.03f;
  } else if (input::is_pressed(input::Actions::DOWN)) {
    velocity.z += 0.03f;
  }

  if (input::is_pressed(input::Actions::LEFT)) {
    velocity.x -= 0.03f;
  } else if (input::is_pressed(input::Actions::RIGHT)) {
    velocity.x += 0.03f;
  }

  player.transform.translation += player.transform.rotation * velocity;

  // camera parent

  camera.transform = player.transform;
}

}  // namespace player
