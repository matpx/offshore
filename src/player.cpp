#include "player.hpp"

#include "input.hpp"
#include "log.hpp"
#include "types.hpp"
#include "world.hpp"

namespace player {

static world::EntityId player_id = world::INVALID_ENTITY;

void setup() {
  world::main_camera = world::create(world::Entity({}, world::Camera(1000, 1000, 1.5, 0.1, 100.0)));
  player_id = world::create(world::Entity({.translation = {{0, 0, 5}}}, world::Player{}));
}

void update() {
  world::Entity& player = world::get(player_id);
  world::Entity& camera = world::get(world::main_camera);

  // rotation

  static vec2 head_rotation = {};

  head_rotation.X += input::last_mouse_motion().X * 0.001f;
  head_rotation.Y += input::last_mouse_motion().Y * 0.001f;

  player.transform.rotation = HMM_QFromAxisAngle_LH(HMM_V3(0, 1, 0), head_rotation.X) *
                              HMM_QFromAxisAngle_LH(HMM_V3(1, 0, 0), head_rotation.Y);

  // translation

  vec3 velocity = {};

  if (input::is_pressed(input::Actions::UP)) {
    velocity.Z -= 0.03f;
  } else if (input::is_pressed(input::Actions::DOWN)) {
    velocity.Z += 0.03f;
  }

  if (input::is_pressed(input::Actions::LEFT)) {
    velocity.X -= 0.03f;
  } else if (input::is_pressed(input::Actions::RIGHT)) {
    velocity.X += 0.03f;
  }

  player.transform.translation += velocity;

  // camera parent

  camera.transform = player.transform;
}

}  // namespace player
