#include "shapes.hpp"
#include "container.hpp"
#include "log.hpp"
#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_shape.h>

#include "unlit.h"

namespace gfx::shapes {

  sg_bindings sphere_binding = {};
  size_t sphere_index_count = 0;

  sg_shader unlit_shader = {};
  sg_pipeline unlit_pipeline = {};

  sshape_vertex_t *vertices = nullptr;
  index_type *indices = nullptr;
  
  void init() {
    DEBUG("init shapes");
  
    arrsetlen(vertices, 128);
    arrsetlen(indices, 512);

    sshape_buffer_t sphere_buffer = {
        .vertices = {
            .buffer = { .ptr = vertices, .size = arrlenu(vertices) * sizeof(sshape_vertex_t) },
        },
        .indices = {
            .buffer = { .ptr = indices, .size = arrlenu(indices) * sizeof(index_type) },
        }
    };

    const sshape_sphere_t params = {
        .radius = 0.5f,
        .slices = 12,
        .stacks = 8,
        .color = sshape_color_4f(1.0f, 0.0f, 0.0f, 1.0f),
        .merge = true,
        .transform = {
            .m = {
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f },
            }
        }
    };

    sphere_buffer = sshape_build_sphere(&sphere_buffer, &params);

    assert(sphere_buffer.valid);

    sg_buffer_desc sphere_vertex_buffer_desc = sshape_vertex_buffer_desc(&sphere_buffer);
    sg_buffer sphere_vertex_buffer = sg_make_buffer(sphere_vertex_buffer_desc);

    sg_buffer_desc sphere_index_buffer_desc = sshape_index_buffer_desc(&sphere_buffer);
    sg_buffer sphere_index_buffer = sg_make_buffer(sphere_index_buffer_desc);

    sshape_element_range_t sphere_element_range = sshape_element_range(&sphere_buffer);
    sphere_index_count = sphere_element_range.num_elements;
    assert(sphere_element_range.base_element == 0);
    assert(sphere_element_range.num_elements > 0);

    sphere_binding = {
      .vertex_buffers = {sphere_vertex_buffer},
      .index_buffer = sphere_index_buffer,
    };

    unlit_shader = sg_make_shader(unlit_shader_desc(sg_query_backend()));

    sg_pipeline_desc unlit_desc = {};
    unlit_desc.shader = unlit_shader;
    unlit_desc.layout.buffers[0] = sshape_buffer_layout_desc();
    unlit_desc.layout.attrs[0] = sshape_position_attr_desc();
    unlit_desc.layout.attrs[1] = sshape_normal_attr_desc();
    unlit_desc.layout.attrs[2] = sshape_texcoord_attr_desc();
    unlit_desc.layout.attrs[3] = sshape_color_attr_desc();
    unlit_desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    unlit_desc.index_type = SG_INDEXTYPE_UINT16;

    unlit_pipeline = sg_make_pipeline(unlit_desc);
  }

  void draw_sphere() {
    sg_apply_pipeline(unlit_pipeline);
    sg_apply_bindings(sphere_binding);
    sg_draw(0, sphere_index_count, 1);
  }

  void finish() {
    DEBUG("finish shapes");
  
    sg_destroy_buffer(sphere_binding.vertex_buffers[0]);
    sg_destroy_buffer(sphere_binding.index_buffer);
    sg_destroy_pipeline(unlit_pipeline);
    sg_destroy_shader(unlit_shader);

    arrfree(vertices);
    arrfree(indices);
  }

}

