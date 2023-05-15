#include "player.hpp"

#include "gfx.hpp"
#include "input.hpp"
#include "material.hpp"
#include "world.hpp"

namespace player {

static world::EntityId player_id = world::INVALID_ENTITY;

void setup() {
  world::main_camera = world::create(world::Entity({}, components::Camera(1000, 1000, 1.5, 0.1, 100.0)));
  player_id = world::create(world::Entity({.translation = {0, 0, 5}}, components::Player{}));

  utils::Array<Vertex, 3> vertices = {
      Vertex{.positions = {0, 0, 0}},
      Vertex{.positions = {2, 0, 0}},
      Vertex{.positions = {0, 2, 0}},
  };

  utils::Array<index_t, 3> indices = {1, 2, 3};

  gfx::Mesh mesh = gfx::create_mesh(vertices, indices);

  world::create(world::Entity({}, components::Renderable{mesh, gfx::material::get()}));
}

void update() {
  world::Entity& player = world::get(player_id);
  world::Entity& camera = world::get(world::main_camera);

  // rotation

  static vec2 head_rotation = {};

  head_rotation.x += input::last_mouse_motion().x * 0.001f;
  head_rotation.y += input::last_mouse_motion().y * 0.001f;

  player.transform.rotation = glm::angleAxis(head_rotation.x, vec3{0, -1, 0}) *
                              glm::angleAxis(head_rotation.y, vec3{-1, 0, 0});

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
