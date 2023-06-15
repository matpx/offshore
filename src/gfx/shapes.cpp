#include "shapes.hpp"

#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_shape.h>

#include <cassert>
#include <entt/entity/registry.hpp>
#include <vector>

#include "../core/log.hpp"
#include "debug.h"
#include "gfx.hpp"

namespace gfx::shapes {

static sg_shader unlit_shader = {};
static sg_pipeline unlit_pipeline = {};

static sg_bindings shapes_bindings = {};

static sshape_element_range_t box_element_range = {};
static sshape_element_range_t sphere_element_range = {};

static bool shapes_pass_active = false;

static void init_pipeline() {
  LOG_DEBUG("gfx::shapes::init_pipeline()");

  assert(unlit_pipeline.id == 0);

  unlit_shader = sg_make_shader(debug_shader_desc(sg_query_backend()));

  sg_pipeline_desc unlit_desc = {};
  unlit_desc.shader = unlit_shader;
  unlit_desc.layout.buffers[0] = sshape_buffer_layout_desc();
  unlit_desc.layout.attrs[0] = sshape_position_attr_desc();
  unlit_desc.layout.attrs[1] = sshape_normal_attr_desc();
  unlit_desc.layout.attrs[2] = sshape_texcoord_attr_desc();
  unlit_desc.layout.attrs[3] = sshape_color_attr_desc();
  unlit_desc.primitive_type = SG_PRIMITIVETYPE_LINES;
  unlit_desc.index_type = SG_INDEXTYPE_UINT16;
  unlit_desc.depth = {
      .compare = SG_COMPAREFUNC_LESS_EQUAL,
      .write_enabled = true,
  };

  unlit_pipeline = sg_make_pipeline(unlit_desc);
}

static void init_buffer() {
  LOG_DEBUG("gfx::shapes::init_buffer()");

  const sshape_box_t box_params = {
      .tiles = 1,
      .merge = true,
  };

  const sshape_sphere_t sphere_params = {.radius = 0.5f,
                                         .slices = 12,
                                         .stacks = 8,
                                         .color = sshape_color_4f(1.0f, 1.0f, 1.0f, 1.0f),
                                         .merge = true,
                                         .transform = {.m = {
                                                           {1.0f, 0.0f, 0.0f, 0.0f},
                                                           {0.0f, 1.0f, 0.0f, 0.0f},
                                                           {0.0f, 0.0f, 1.0f, 0.0f},
                                                           {0.0f, 0.0f, 0.0f, 1.0f},
                                                       }}};

  const sshape_sizes_t box_size = sshape_box_sizes(box_params.tiles);
  const sshape_sizes_t sphere_size = sshape_sphere_sizes(sphere_params.slices, sphere_params.stacks);

  std::vector<sshape_vertex_t> vertices(box_size.vertices.size + sphere_size.vertices.size);
  std::vector<index_t> indices(box_size.indices.size + sphere_size.indices.size);

  sshape_buffer_t shapes_merge_buffer = {
      .vertices =
          {
              .buffer = {.ptr = vertices.data(), .size = vertices.size() * sizeof(sshape_vertex_t)},
          },
      .indices = {
          .buffer = {.ptr = indices.data(), .size = indices.size() * sizeof(index_t)},
      }};

  shapes_merge_buffer = sshape_build_box(&shapes_merge_buffer, &box_params);
  assert(shapes_merge_buffer.valid);

  box_element_range = sshape_element_range(&shapes_merge_buffer);

  shapes_merge_buffer = sshape_build_sphere(&shapes_merge_buffer, &sphere_params);
  assert(shapes_merge_buffer.valid);

  sphere_element_range = sshape_element_range(&shapes_merge_buffer);
  sphere_element_range.base_element = box_element_range.num_elements;
  sphere_element_range.num_elements -= box_element_range.num_elements;

  const sg_buffer_desc shapes_vertex_buffer_desc = sshape_vertex_buffer_desc(&shapes_merge_buffer);
  const sg_buffer shapes_vertex_buffer = sg_make_buffer(shapes_vertex_buffer_desc);

  const sg_buffer_desc shapes_index_buffer_desc = sshape_index_buffer_desc(&shapes_merge_buffer);
  const sg_buffer shapes_index_buffer = sg_make_buffer(shapes_index_buffer_desc);

  shapes_bindings = {
      .vertex_buffers = {shapes_vertex_buffer},
      .index_buffer = shapes_index_buffer,
  };
}

void init() {
  LOG_INFO("gfx::shapes::init()");

  init_pipeline();
  init_buffer();
}

void begin_pass() {
  assert(!shapes_pass_active);

  sg_apply_pipeline(unlit_pipeline);
  sg_apply_bindings(shapes_bindings);

  shapes_pass_active = true;
}

static void draw_shape(const sshape_element_range_t& element_range, const vec3& center, const vec3& scale) {
  assert(shapes_pass_active);

  const mat4 mvp = gfx::get_current_vp() * glm::scale(glm::translate(glm::identity<mat4>(), center), scale);

  MVP_t MVP = {
      .mvp = mvp,
  };
  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_MVP, SG_RANGE(MVP));

  sg_draw(element_range.base_element, element_range.num_elements, 1);
}

void draw_box(const vec3& center, const vec3& scale) { draw_shape(box_element_range, center, scale); }

void draw_sphere(const vec3& center, const vec3& scale) { draw_shape(sphere_element_range, center, scale); }

void draw_aabb(const AABB& aabb) {
  draw_box(aabb.center, aabb.extent * 2.0f);
}

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

  sg_destroy_buffer(shapes_bindings.vertex_buffers[0]);
  sg_destroy_buffer(shapes_bindings.index_buffer);

  sg_destroy_pipeline(unlit_pipeline);
  sg_destroy_shader(unlit_shader);

  LOG_INFO("gfx::shapes::finish()");
}

}  // namespace gfx::shapes