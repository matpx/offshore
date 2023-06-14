#include "terrain.hpp"

#include <rjm/rjm_mc.h>
#include <stb/stb_perlin.h>

#include <entt/entity/registry.hpp>
#include <limits>

#include "../core/log.hpp"
#include "../gfx/gfx.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems {

static float testIsoFn(const float *position, [[maybe_unused]] float *extra, [[maybe_unused]] void *userparam) {
  // const vec3 xn = glm::normalize(x) * 2.0f;
  // return glm::length2(x) - 90 - std::max(0.0f, stb_perlin_noise3(xn.x, xn.y, xn.z, 0, 0, 0) * 10.0f);

  const vec3 xv = vec3{position[0], position[1], position[2]} * 2.0f;

  return std::sin(xv.x) + std::sin(xv.y) + std::sin(xv.z) - 1.5f;
}

void Terrain::setup() {
  LOG_DEBUG("terrain create");

  float bmin[3] = {-10, -10, -10};
  float bmax[3] = {+10, +10, +10};
  float res = 0.5f;

  McMesh iso_mesh = mcGenerate(bmin, bmax, res, testIsoFn, NULL);

  LOG_DEBUG("terrain verts: %d", iso_mesh.nverts);
  LOG_DEBUG("terrain indices: %d", iso_mesh.ntris * 3);

  std::vector<gfx::Vertex> vertex_data(iso_mesh.nverts);
  std::vector<gfx::index_t> index_data(iso_mesh.ntris * 3);

  for (int n = 0; n < iso_mesh.nverts; n++) {
    vertex_data[n] = {
        .positions = {iso_mesh.verts[n].x, iso_mesh.verts[n].y, iso_mesh.verts[n].z},
        .normal = {iso_mesh.verts[n].nx, iso_mesh.verts[n].ny, iso_mesh.verts[n].nz},
    };
  }

  for (int n = 0; n < iso_mesh.ntris; n++) {
    index_data[n * 3 + 0] = iso_mesh.indices[n * 3 + 0];
    index_data[n * 3 + 1] = iso_mesh.indices[n * 3 + 1];
    index_data[n * 3 + 2] = iso_mesh.indices[n * 3 + 2];
  }

  assert(vertex_data.size() > 0 && vertex_data.size() < std::numeric_limits<gfx::index_t>::max());
  assert(index_data.size() > 0);

  const gfx::Mesh mesh = gfx::create_mesh(vertex_data, index_data);

  const entt::entity terrain_chunk = world::registry->create();

  world::registry->emplace<comp::Transform>(terrain_chunk, comp::Transform{});
  world::registry->emplace<comp::Renderable>(terrain_chunk, comp::Renderable{mesh, gfx::material::get()});

  vertex_data.clear();
  index_data.clear();

  mcFree(&iso_mesh);
}

}  // namespace systems