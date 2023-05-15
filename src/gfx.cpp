#include "gfx.hpp"

#include <SDL2/SDL.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/util/sokol_color.h>

#include "shapes.hpp"
#include "unlit.h"
#include "utils.hpp"

namespace gfx {

static_assert(sizeof(index_t) == 2);

// Context
SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;

u32 window_width = 1200;
u32 window_height = 800;

// Pipelines
sg_shader unlit_shader = {};
sg_pipeline basic_pipeline = {};

// State
mat4 current_vp;

void init_pipelines() {
  LOG_INFO("gfx::init_pipelines()");

  unlit_shader = sg_make_shader(unlit_shader_desc(sg_query_backend()));

  sg_pipeline_desc basic_desc = {};
  basic_desc.shader = unlit_shader;
  basic_desc.layout.attrs[ATTR_vs_position] = {.format = SG_VERTEXFORMAT_FLOAT3};
  basic_desc.index_type = SG_INDEXTYPE_UINT16;

  basic_pipeline = sg_make_pipeline(basic_desc);
}

void finish_pipelines() {
  sg_destroy_pipeline(basic_pipeline);
  sg_destroy_shader(unlit_shader);

  LOG_INFO("gfx::finish_pipelines()");
}

void init() {
  LOG_INFO("gfx::init()");

  assert(window == nullptr && context == nullptr);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    FATAL("SDL_Init() failed");
  }

  window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    FATAL("SDL_CreateWindow() failed");
  }

  context = SDL_GL_CreateContext(window);

  if (context == nullptr) {
    FATAL("SDL_GL_CreateContext() failed");
  }

  sg_desc desc = {.logger = {.func = slog_func}};

  desc.allocator = {
      .alloc = [](size_t size, [[maybe_unused]] void* user_data) { return allocator::_malloc(size); },
      .free = [](void* ptr, [[maybe_unused]] void* user_data) { allocator::_free(ptr); },
  };

  sg_setup(desc);

  init_pipelines();
  shapes::init();
}

Mesh create_mesh(const utils::Span<Vertex> vertex_data, const utils::Span<index_t> index_data) {
  const sg_buffer_desc vertex_buffer_desc = {
      .data = {
          .ptr = vertex_data.data(),
          .size = vertex_data.size() * sizeof(Vertex),
      }};

  sg_buffer vertex_buffer = sg_make_buffer(vertex_buffer_desc);

  const sg_buffer_desc index_buffer_desc = {
      .type = SG_BUFFERTYPE_INDEXBUFFER,
      .data = {
          .ptr = index_data.data(),
          .size = index_data.size() * sizeof(index_t),
      },
  };

  sg_buffer index_buffer = sg_make_buffer(index_buffer_desc);

  return {
      .bindings = {
          .vertex_buffers = {vertex_buffer},
          .index_buffer = index_buffer},
      .num_elements = static_cast<u32>(index_data.size()),
  };
}

void begin_frame(world::Entity& camera) {
  const sg_pass_action pass_action = {
      .colors = {{
          .action = SG_ACTION_CLEAR,
          .value = SG_BLACK,
      }},
  };

  sg_begin_default_pass(&pass_action, window_width, window_height);

  camera.transform.update();

  components::Camera& camera_component = camera.get_camera();

  if (camera_component.width != window_width || camera_component.height != window_height) {
    LOG_DEBUG("camera_component.update()");
    camera_component.width = window_width;
    camera_component.height = window_height;
    camera_component.update();
  }

  current_vp = camera_component.projection * glm::inverse(camera.transform.world);
}

void draw_world() {
  const utils::Span<world::Entity> entities = world::get_entities();

  VP_t VP = {
      .vp = current_vp,
  };

  sg_apply_pipeline(basic_pipeline);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_VP, SG_RANGE(VP));

  for (size_t i = 0; i < entities.size(); i++) {
    const world::Entity& entity = entities[i];

    if (entity.variant != world::Entity::Variant::Renderable) {
      continue;
    }

    const Mesh& mesh = entity.renderable.mesh;

    sg_apply_bindings(mesh.bindings);
    sg_draw(0, mesh.num_elements, 1);
  }
}

void end_frame() {
  sg_end_pass();
  sg_commit();
}

void present() {
  SDL_GL_SwapWindow(window);
}

void finish() {
  shapes::finish();
  finish_pipelines();

  sg_shutdown();

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);

  // assert(glGetError() == 0);
  assert(SDL_GetError()[0] == '\0');

  SDL_Quit();

  LOG_INFO("gfx::finish()");
}

}  // namespace gfx
