#include "terrain.hpp"

#include <rjm/rjm_mc.h>
#include <stb/stb_perlin.h>

#include <cassert>
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

  const vec3 base_offset = *((vec3 *)userparam);
  const vec3 xv = base_offset + vec3{position[0], position[1], position[2]};

  return std::sin(glm::length(xv) * 1.0f);
}

gfx::Mesh build_chunk(const vec3 base_offset) {
  LOG_DEBUG("chunk create");

  constexpr float radius = 8.0f;

  float bmin[3] = {-radius, -radius, -radius};
  float bmax[3] = {+radius, +radius, +radius};
  float res = 0.5f;

  McMesh iso_mesh = mcGenerate(bmin, bmax, res, testIsoFn, (void *)&base_offset);

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

  assert(vertex_data.size() > 0);
  assert(vertex_data.size() < std::numeric_limits<gfx::index_t>::max());
  assert(index_data.size() > 0);

  const gfx::Mesh mesh = gfx::create_mesh(vertex_data, index_data);

  mcFree(&iso_mesh);

  return mesh;
}

void Terrain::setup() {
  LOG_DEBUG("terrain create");

  const vec3 base_offset = {0, 0, 0};

  const gfx::Mesh mesh = build_chunk(base_offset);
  const entt::entity terrain_chunk = world::registry->create();

  world::registry->emplace<comp::Transform>(terrain_chunk, comp::Transform{.translation = base_offset});
  world::registry->emplace<comp::Renderable>(terrain_chunk, comp::Renderable{mesh, gfx::material::get()});
}

}  // namespace systems