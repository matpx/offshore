#include "terrain.hpp"

#include <functional>

#include "../gfx/gfx.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems::terrain {

constexpr u16 dim = 4;
constexpr float radius = 2;

gfx::Mesh make_plane(const std::function<vec3(vec3)>& remapper, const bool inverted_indices) {
  container::Vector<gfx::Vertex> vertex_data;
  container::Vector<gfx::index_t> index_data;

  for (i32 x = 0; x < dim; x++) {
    for (i32 y = 0; y < dim; y++) {
      vec3 remapped;

      remapped = remapper({x, y, 0});
      vertex_data.push(gfx::Vertex{remapped.x, remapped.y, remapped.z});

      remapped = remapper({x + 1, y, 0});
      vertex_data.push(gfx::Vertex{{remapped.x, remapped.y, remapped.z}});

      remapped = remapper({x, y + 1, 0});
      vertex_data.push(gfx::Vertex{{remapped.x, remapped.y, remapped.z}});

      remapped = remapper({x + 1, y + 1, 0});
      vertex_data.push(gfx::Vertex{{remapped.x, remapped.y, remapped.z}});

      remapped = remapper({x, y + 1, 0});
      vertex_data.push(gfx::Vertex{{remapped.x, remapped.y, remapped.z}});

      remapped = remapper({x + 1, y, 0});
      vertex_data.push(gfx::Vertex{{remapped.x, remapped.y, remapped.z}});
    }
  }

  if (inverted_indices) {
    for (i32 i = dim * dim * 6 - 1; i >= 0; i--) {
      index_data.push(i);
    }
  } else {
    for (i32 i = 0; i < dim * dim * 6; i++) {
      index_data.push(i);
    }
  }

  gfx::Mesh mesh = gfx::create_mesh(vertex_data, index_data);

  vertex_data.clear();
  index_data.clear();

  return mesh;
}

void create() {
  constexpr float hd = dim / 2.0f;

  gfx::Mesh m;

  m = make_plane([](vec3 i) -> vec3 { return glm::normalize(vec3{i.x - hd, i.y - hd, hd}) * radius; }, false);
  world::create(world::Entity({}, components::Renderable{m, gfx::material::get()}));

  m = make_plane([](vec3 i) -> vec3 { return glm::normalize(vec3{i.x - hd, i.y - hd, -hd}) * radius; }, true);
  world::create(world::Entity({}, components::Renderable{m, gfx::material::get()}));

  m = make_plane([](vec3 i) -> vec3 { return glm::normalize(vec3{i.x - hd, -hd, i.y - hd}) * radius; }, false);
  world::create(world::Entity({}, components::Renderable{m, gfx::material::get()}));

  m = make_plane([](vec3 i) -> vec3 { return glm::normalize(vec3{i.x - hd, hd, i.y - hd}) * radius; }, true);
  world::create(world::Entity({}, components::Renderable{m, gfx::material::get()}));

  m = make_plane([](vec3 i) -> vec3 { return glm::normalize(vec3{-hd, i.y - hd, i.x - hd}) * radius; }, false);
  world::create(world::Entity({}, components::Renderable{m, gfx::material::get()}));

  m = make_plane([](vec3 i) -> vec3 { return glm::normalize(vec3{hd, i.y - hd, i.x - hd}) * radius; }, true);
  world::create(world::Entity({}, components::Renderable{m, gfx::material::get()}));
}

}  // namespace systems::terrain