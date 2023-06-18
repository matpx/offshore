#include "shapes.hpp"

#include <sokol/sokol_gfx.h>

#include <array>
#include <cassert>
#include <entt/entity/registry.hpp>
#include <vector>

#include "../core/log.hpp"
#include "debug.h"
#include "gfx.hpp"
#include "vertex.hpp"

namespace gfx::shapes {

const static std::array<SimpleVertex, 24> cube_vertex_data = {
    SimpleVertex{{-0.5, -0.5f, -0.5f}}, SimpleVertex{{0.5, -0.5f, -0.5f}},  SimpleVertex{{0.5, -0.5f, -0.5f}},
    SimpleVertex{{0.5, 0.5f, -0.5f}},   SimpleVertex{{0.5, 0.5f, -0.5f}},   SimpleVertex{{-0.5, 0.5f, -0.5f}},
    SimpleVertex{{-0.5, 0.5f, -0.5f}},  SimpleVertex{{-0.5, -0.5f, -0.5f}}, SimpleVertex{{-0.5, -0.5f, 0.5f}},
    SimpleVertex{{0.5, -0.5f, 0.5f}},   SimpleVertex{{0.5, -0.5f, 0.5f}},   SimpleVertex{{0.5, 0.5f, 0.5f}},
    SimpleVertex{{0.5, 0.5f, 0.5f}},    SimpleVertex{{-0.5, 0.5f, 0.5f}},   SimpleVertex{{-0.5, 0.5f, 0.5f}},
    SimpleVertex{{-0.5, -0.5f, 0.5f}},  SimpleVertex{{-0.5, -0.5f, -0.5f}}, SimpleVertex{{-0.5, -0.5f, 0.5f}},
    SimpleVertex{{0.5, -0.5f, -0.5f}},  SimpleVertex{{0.5, -0.5f, 0.5f}},   SimpleVertex{{0.5, 0.5f, -0.5f}},
    SimpleVertex{{0.5, 0.5f, 0.5f}},    SimpleVertex{{-0.5, 0.5f, -0.5f}},  SimpleVertex{{-0.5, 0.5f, 0.5f}},
};

static sg_shader unlit_shader = {};
static sg_pipeline unlit_pipeline = {};

static sg_bindings cube_bindings = {};

static bool shapes_pass_active = false;

static void init_pipeline() {
  LOG_DEBUG("gfx::shapes::init_pipeline()");

  assert(unlit_pipeline.id == 0);

  unlit_shader = sg_make_shader(debug_shader_desc(sg_query_backend()));

  sg_pipeline_desc unlit_desc = {};
  unlit_desc.shader = unlit_shader;
  unlit_desc.layout.attrs[ATTR_vs_position] = {.format = SG_VERTEXFORMAT_FLOAT3};
  unlit_desc.primitive_type = SG_PRIMITIVETYPE_LINES;
  unlit_desc.index_type = SG_INDEXTYPE_NONE;
  unlit_desc.depth = {
      .compare = SG_COMPAREFUNC_LESS_EQUAL,
      .write_enabled = true,
  };

  unlit_pipeline = sg_make_pipeline(unlit_desc);
}

static void init_buffer() {
  LOG_DEBUG("gfx::shapes::init_buffer()");

  const sg_buffer_desc vertex_buffer_desc = {
      .type = SG_BUFFERTYPE_VERTEXBUFFER,
      .data =
          {
              .ptr = cube_vertex_data.data(),
              .size = cube_vertex_data.size() * sizeof(SimpleVertex),
          },
  };

  cube_bindings.vertex_buffers[0] = sg_make_buffer(vertex_buffer_desc);
}

void init() {
  LOG_INFO("gfx::shapes::init()");

  init_pipeline();
  init_buffer();
}

void begin_pass() {
  assert(!shapes_pass_active);

  sg_apply_pipeline(unlit_pipeline);
  sg_apply_bindings(cube_bindings);

  shapes_pass_active = true;
}

static void draw_shape(i32 base_element, i32 num_elements, const vec3& center, const vec3& scale) {
  assert(shapes_pass_active);

  const mat4 mvp = gfx::get_current_vp() * glm::scale(glm::translate(glm::identity<mat4>(), center), scale);

  MVP_t MVP = {
      .mvp = mvp,
  };
  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_MVP, SG_RANGE(MVP));

  sg_draw(base_element, num_elements, 1);
}

void draw_box(const vec3& center, const vec3& scale) { draw_shape(0, cube_vertex_data.size(), center, scale); }

void draw_aabb(const geometry::AABB& aabb) { draw_box(aabb.center, aabb.extent * 2.0f); }

void draw_all_aabb() {
  for (const auto [entity, transform] : world::registry->view<comp::Transform>().each()) {
    draw_aabb(transform.global_aabb);
  }
}

void finish_pass() {
  assert(shapes_pass_active);

  shapes_pass_active = false;
}

void finish() {
  assert(!shapes_pass_active);

  sg_destroy_buffer(cube_bindings.vertex_buffers[0]);

  sg_destroy_pipeline(unlit_pipeline);
  sg_destroy_shader(unlit_shader);

  LOG_INFO("gfx::shapes::finish()");
}

}  // namespace gfx::shapes