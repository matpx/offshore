#include "terrain.hpp"

#include <rjm/rjm_mc.h>
#include <stb/stb_perlin.h>

#include <cassert>
#include <entt/entity/registry.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include "../core/log.hpp"
#include "../gfx/gfx.hpp"
#include "../gfx/material.hpp"
#include "../world/world.hpp"

namespace systems {

constexpr float base_radius = 8.0f;

struct UserParam {
  vec3 base_offset;
  float radius;
};

static float testIsoFn(const float *position, [[maybe_unused]] float *extra, [[maybe_unused]] void *userparam) {
  // const vec3 xn = glm::normalize(x) * 2.0f;
  // return glm::length2(x) - 90 - std::max(0.0f, stb_perlin_noise3(xn.x, xn.y, xn.z, 0, 0, 0) * 10.0f);

  UserParam *up = (UserParam *)userparam;

  const vec3 base_offset = up->base_offset;
  const vec3 local_offset = vec3{position[0], position[1], position[2]};
  const vec3 xv = base_offset + local_offset;

  return glm::length(xv) - 100.0f;
}

void build_chunk(const vec3 &base_offset, const vec3 &chunk_offset, const u32 level) {
  assert(level > 0);

  const float radius = base_radius * level;

  const vec3 local_offset = chunk_offset * (radius * 2.0f);

  const float bmin[3] = {-radius, -radius, -radius};
  const float bmax[3] = {+radius, +radius, +radius};
  const float res = 1.0f * level;

  UserParam up = {
      .base_offset = base_radius * base_offset + local_offset,
      .radius = radius,
  };

  McMesh iso_mesh = mcGenerate(bmin, bmax, res, testIsoFn, (void *)&up);

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

  if (vertex_data.size() > 0 && vertex_data.size() < std::numeric_limits<gfx::index_t>::max() &&
      index_data.size() > 0) {
    LOG_DEBUG("vertices: %d, indices: %d", iso_mesh.nverts, iso_mesh.ntris * 3);

    const gfx::Mesh mesh = gfx::create_mesh(vertex_data, index_data);
    const entt::entity terrain_chunk = world::registry->create();

    world::registry->emplace<comp::TerrainChunk>(terrain_chunk, comp::TerrainChunk{});
    world::registry->emplace<comp::Transform>(terrain_chunk,
                                              comp::Transform{
                                                  .translation = base_radius * base_offset + local_offset,
                                              });
    world::registry->emplace<comp::Renderable>(terrain_chunk, comp::Renderable{mesh, gfx::material::get()});
  } else {
    LOG_DEBUG("chunk skipped");
  }

  mcFree(&iso_mesh);
}

void rebuild(const vec3 &root_center) {
  LOG_DEBUG("terrain rebuild");

  build_chunk(root_center, vec3{0, 0, 0}, 1);

  for (i32 x = -1; x <= 1; x++) {
    for (i32 y = -1; y <= 1; y++) {
      for (i32 z = -1; z <= 1; z++) {
        if (x == 0 && y == 0 && z == 0) continue;

        build_chunk(root_center, vec3{x, y, z}, 1);
      }
    }
  }

  for (i32 x = -1; x <= 1; x++) {
    for (i32 y = -1; y <= 1; y++) {
      for (i32 z = -1; z <= 1; z++) {
        if (x == 0 && y == 0 && z == 0) continue;

        build_chunk(root_center, vec3{x, y, z}, 3);
      }
    }
  }

  for (i32 x = -1; x <= 1; x++) {
    for (i32 y = -1; y <= 1; y++) {
      for (i32 z = -1; z <= 1; z++) {
        if (x == 0 && y == 0 && z == 0) continue;

        build_chunk(root_center, vec3{x, y, z}, 9);
      }
    }
  }
}

void Terrain::update(double delta_time) {
  static ivec3 root_center = {0, 0, 0};
  static bool present = false;

  if (!present) {
    rebuild(root_center);

    present = true;
  } else {
    ivec3 camera_translation = world::registry->get<comp::Transform>(world::main_camera).world[3];
    camera_translation /= base_radius;

    const ivec3 dist = camera_translation - root_center;

    if (dist.x != 0 || dist.y != 0 || dist.z != 0) {
      LOG_DEBUG("delete terrain");
      LOG_DEBUG("%s", glm::to_string(root_center + dist).c_str());

      for (const auto &[entity, renderable] : world::registry->view<comp::Renderable, comp::TerrainChunk>().each()) {
        gfx::destroy_mesh(renderable.mesh);

        world::registry->destroy(entity);
      }

      root_center = camera_translation;

      rebuild(root_center);
    }
  }
}

}  // namespace systems