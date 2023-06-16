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

constexpr float base_radius = 32.0f;

struct UserParams {
  vec3 global_offset;
  float scaled_radius;
};

static float testIsoFn(const float *position, [[maybe_unused]] float *extra, [[maybe_unused]] void *userparam) {
  // const vec3 xn = glm::normalize(x) * 2.0f;
  // return glm::length2(x) - 90 - std::max(0.0f, stb_perlin_noise3(xn.x, xn.y, xn.z, 0, 0, 0) * 10.0f);

  UserParams *up = (UserParams *)userparam;

  const vec3 base_offset = up->global_offset;
  const vec3 local_offset = vec3{position[0], position[1], position[2]};
  const vec3 xv = base_offset + local_offset;

  return glm::length(xv) - 100.0f;
}

ivec2 build_chunk(const vec3 &base_offset, const vec3 &chunk_offset, const u32 level) {
  assert(level > 0);

  const float scaled_radius = base_radius * level;

  const vec3 local_offset = chunk_offset * (scaled_radius * 2.0f);
  const vec3 global_offset = base_radius * base_offset + local_offset;

  const float bmin[3] = {-scaled_radius, -scaled_radius, -scaled_radius};
  const float bmax[3] = {+scaled_radius, +scaled_radius, +scaled_radius};
  const float res = 4.0f * level;

  UserParams up = {
      global_offset,
      scaled_radius,
  };

  McMesh iso_mesh = mcGenerate(bmin, bmax, res, testIsoFn, (void *)&up);

  std::vector<gfx::Vertex> vertex_data(iso_mesh.nverts);
  std::vector<gfx::index_t> index_data(iso_mesh.ntris * 3);

  for (size_t n = 0; n < vertex_data.size(); n++) {
    vertex_data[n] = {
        .positions = {iso_mesh.verts[n].x, iso_mesh.verts[n].y, iso_mesh.verts[n].z},
        .normal = {iso_mesh.verts[n].nx, iso_mesh.verts[n].ny, iso_mesh.verts[n].nz},
    };
  }

  for (size_t n = 0; n < index_data.size(); n += 3) {
    index_data[n + 0] = iso_mesh.indices[n + 0];
    index_data[n + 1] = iso_mesh.indices[n + 1];
    index_data[n + 2] = iso_mesh.indices[n + 2];
  }

  assert(vertex_data.size() < std::numeric_limits<gfx::index_t>::max());

  if (vertex_data.size() > 0 && index_data.size() > 0) {
    const gfx::Mesh mesh = gfx::create_mesh(vertex_data, index_data);
    const entt::entity terrain_chunk = world::registry->create();

    world::registry->emplace<comp::TerrainChunk>(terrain_chunk, comp::TerrainChunk{});
    world::registry->emplace<comp::Transform>(terrain_chunk, comp::Transform{
                                                                 .translation = global_offset,
                                                             });
    world::registry->emplace<comp::Renderable>(terrain_chunk, comp::Renderable{mesh, gfx::material::get()});
  }

  mcFree(&iso_mesh);

  return {vertex_data.size(), index_data.size()};
}

void rebuild(const vec3 &root_center) {
  ivec2 vertex_index_count = {0, 0};

  vertex_index_count += build_chunk(root_center, vec3{0, 0, 0}, 1);

  for (i32 x = -1; x <= 1; x++) {
    for (i32 y = -1; y <= 1; y++) {
      for (i32 z = -1; z <= 1; z++) {
        if (x == 0 && y == 0 && z == 0) continue;

        vertex_index_count += build_chunk(root_center, vec3{x, y, z}, 1);
      }
    }
  }

  for (i32 x = -1; x <= 1; x++) {
    for (i32 y = -1; y <= 1; y++) {
      for (i32 z = -1; z <= 1; z++) {
        if (x == 0 && y == 0 && z == 0) continue;

        vertex_index_count += build_chunk(root_center, vec3{x, y, z}, 3);
      }
    }
  }

  for (i32 x = -1; x <= 1; x++) {
    for (i32 y = -1; y <= 1; y++) {
      for (i32 z = -1; z <= 1; z++) {
        if (x == 0 && y == 0 && z == 0) continue;

        vertex_index_count += build_chunk(root_center, vec3{x, y, z}, 9);
      }
    }
  }

  LOG_DEBUG("terrain rebuild, vertices: %d, indices: %d", vertex_index_count.x, vertex_index_count.y);
}

void Terrain::update([[maybe_unused]] double delta_time) {
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