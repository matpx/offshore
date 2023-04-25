#include "gfx.hpp"
#include "alloc.hpp"
#include <SDL2/SDL.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol/sokol_gfx.h"
#include "sokol/util/sokol_shape.h"

#include "unlit.h"

namespace gfx {

  sg_bindings sphere_binding = {};
  size_t sphere_index_count = 0;
  sg_pipeline unlit_pipeline = {};

  void init_shapes() {
    sshape_vertex_t vertices[512];
    uint16_t indices[4096];

    sshape_buffer_t sphere_buffer = {
        .vertices = {
            .buffer = SSHAPE_RANGE(vertices),
        },
        .indices = {
            .buffer = SSHAPE_RANGE(indices),
        }
    };

    const sshape_sphere_t params = {
        .radius = 0.5f,
        .slices = 16,
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
    sg_buffer sphere_vertex_buffer = sg_make_buffer(&sphere_vertex_buffer_desc);

    sg_buffer_desc sphere_index_buffer_desc = sshape_index_buffer_desc(&sphere_buffer);
    sg_buffer sphere_index_buffer = sg_make_buffer(&sphere_index_buffer_desc);

    sshape_element_range_t sphere_element_range = sshape_element_range(&sphere_buffer);
    sphere_index_count = sphere_element_range.num_elements;
    assert(sphere_element_range.base_element == 0);
    assert(sphere_element_range.num_elements > 0);

    sphere_binding = {
      .vertex_buffers = {sphere_vertex_buffer},
      .index_buffer = sphere_index_buffer,
    };

    sg_shader unlit_shader = sg_make_shader(unlit_shader_desc(sg_query_backend()));

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

  SDL_Window* window    = nullptr;
  SDL_GLContext context = nullptr;

  SDL_Window* init() {
    assert(window == nullptr && context == nullptr);
  
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    sg_desc desc = {
      .allocator = {
          .alloc = [] (size_t size, [[maybe_unused]] void* user_data) { return omalloc(size); },
          .free =  [] (void* ptr, [[maybe_unused]] void* user_data)   { ofree(ptr); },
      }
    };
    
    sg_setup(desc);

    init_shapes();

    return window;
  }

  void begin_frame() {
    sg_pass_action pass_action = {};
    sg_begin_default_pass(&pass_action, 1200, 800);
  }

  void draw_sphere() {
    sg_apply_pipeline(unlit_pipeline);
    sg_apply_bindings(&sphere_binding);
    sg_draw(0, sphere_index_count, 1);
  }

  void end_frame() {
    sg_end_pass();
    sg_commit();
  }

  void swap() {
    SDL_GL_SwapWindow(window);
  }

  void finish() {
    sg_shutdown();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    assert(glGetError() == 0);
    assert(SDL_GetError()[0] == '\0');
    
    SDL_Quit();
  }

}

