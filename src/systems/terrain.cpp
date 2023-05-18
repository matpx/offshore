#include "terrain.hpp"

#include "../gfx/gfx.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems::terrain {

/*
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
*/

#define MC_IMPLEMENTATION
#include <rjm/rjm_mc.h>

float testIsoFn(const float *pos, [[maybe_unused]] float *extra, [[maybe_unused]] void *userparam) {
  float x = pos[0], y = pos[1], z = pos[2];
  return (x * x + y * y + z * z) - 5;
}

void create() {
  LOG_DEBUG("terrain create");

  float bmin[3] = {-2, -2, -2};
  float bmax[3] = {+2, +2, +2};
  float res = 0.5f;

  McMesh iso_mesh = mcGenerate(bmin, bmax, res, testIsoFn, NULL);

  LOG_DEBUG("terrain verts: %d", iso_mesh.nverts);
  LOG_DEBUG("terrain indices: %d", iso_mesh.ntris * 3);

  container::Vector<gfx::Vertex> vertex_data(iso_mesh.nverts);
  container::Vector<gfx::index_t> index_data(iso_mesh.ntris * 3);

  for (int n = 0; n < iso_mesh.nverts; n++) {
    vertex_data[n] = {{iso_mesh.verts[n].x, iso_mesh.verts[n].y, iso_mesh.verts[n].z}};
  }

  for (int n = 0; n < iso_mesh.ntris; n++) {
    index_data[n * 3 + 0] = iso_mesh.indices[n * 3 + 0];
    index_data[n * 3 + 1] = iso_mesh.indices[n * 3 + 1];
    index_data[n * 3 + 2] = iso_mesh.indices[n * 3 + 2];
  }

  const gfx::Mesh mesh = gfx::create_mesh(vertex_data, index_data);

  world::create(world::Entity({}, components::Renderable{mesh, gfx::material::get()}));

  vertex_data.clear();
  index_data.clear();

  mcFree(&iso_mesh);
}

}  // namespace systems::terrain