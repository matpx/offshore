#include "shapes.hpp"

#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_shape.h>

#include "gfx.hpp"
#include "unlit.h"
#include "../core/container.hpp"

namespace gfx::shapes {

static Mesh sphere_mesh = {};

static sg_shader unlit_shader = {};
static sg_pipeline unlit_pipeline = {};

void init() {
  LOG_INFO("gfx::shapes::init()");

  unlit_shader = sg_make_shader(unlit_shader_desc(sg_query_backend()));

  sg_pipeline_desc unlit_desc = {};
  unlit_desc.shader = unlit_shader;
  unlit_desc.layout.buffers[0] = sshape_buffer_layout_desc();
  unlit_desc.layout.attrs[0] = sshape_position_attr_desc();
  unlit_desc.layout.attrs[1] = sshape_normal_attr_desc();
  unlit_desc.layout.attrs[2] = sshape_texcoord_attr_desc();
  unlit_desc.layout.attrs[3] = sshape_color_attr_desc();
  unlit_desc.primitive_type = SG_PRIMITIVETYPE_LINES;
  unlit_desc.index_type = SG_INDEXTYPE_UINT16;

  unlit_pipeline = sg_make_pipeline(unlit_desc);

  container::Vector<sshape_vertex_t> vertices(128);
  container::Vector<index_t> indices(512);

  sshape_buffer_t sphere_buffer = {
      .vertices = {
          .buffer = {
              .ptr = vertices.data(),
              .size = vertices.size() * sizeof(sshape_vertex_t)},
      },
      .indices = {
          .buffer = {.ptr = indices.data(), .size = indices.size() * sizeof(index_t)},
      }};

  const sshape_sphere_t params = {
      .radius = 0.5f,
      .slices = 12,
      .stacks = 8,
      .color = sshape_color_4f(1.0f, 1.0f, 1.0f, 1.0f),
      .merge = true,
      .transform = {
          .m = {
              {1.0f, 0.0f, 0.0f, 0.0f},
              {0.0f, 1.0f, 0.0f, 0.0f},
              {0.0f, 0.0f, 1.0f, 0.0f},
              {0.0f, 0.0f, 0.0f, 1.0f},
          }}};

  sphere_buffer = sshape_build_sphere(&sphere_buffer, &params);
  assert(sphere_buffer.valid);

  const sg_buffer_desc sphere_vertex_buffer_desc = sshape_vertex_buffer_desc(&sphere_buffer);
  const sg_buffer sphere_vertex_buffer = sg_make_buffer(sphere_vertex_buffer_desc);

  const sg_buffer_desc sphere_index_buffer_desc = sshape_index_buffer_desc(&sphere_buffer);
  const sg_buffer sphere_index_buffer = sg_make_buffer(sphere_index_buffer_desc);

  vertices.clear();
  indices.clear();

  sphere_mesh.num_elements = sshape_element_range(&sphere_buffer).num_elements;

  sphere_mesh.bindings = {
      .vertex_buffers = {sphere_vertex_buffer},
      .index_buffer = sphere_index_buffer,
  };
}

void draw_sphere() {
  sg_apply_pipeline(unlit_pipeline);
  sg_apply_bindings(sphere_mesh.bindings);

  VP_t VP = {
      .vp = current_vp,
  };
  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_VP, SG_RANGE(VP));

  sg_draw(0, sphere_mesh.num_elements, 1);
}

void finish() {
  sg_destroy_buffer(sphere_mesh.bindings.vertex_buffers[0]);
  sg_destroy_buffer(sphere_mesh.bindings.index_buffer);

  sg_destroy_pipeline(unlit_pipeline);
  sg_destroy_shader(unlit_shader);

  LOG_INFO("gfx::shapes::finish()");
}

}  // namespace gfx::shapes