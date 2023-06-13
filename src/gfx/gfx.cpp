#include "gfx.hpp"

#include <SDL2/SDL.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/util/sokol_color.h>

#include <entt/entity/registry.hpp>

#include "../core/log.hpp"
#include "material.hpp"
#include "shapes.hpp"
#include "ui.hpp"

namespace gfx {

static_assert(sizeof(index_t) == 2);

static SDL_Window* window = nullptr;
static SDL_GLContext context = nullptr;

static u32 window_width = 1200;
static u32 window_height = 800;

mat4 current_vp;

void init() {
  LOG_INFO("gfx::init()");

  assert(window == nullptr && context == nullptr);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    FATAL("SDL_Init() failed");
  }

  SDL_version sdl_version;
  SDL_GetVersion(&sdl_version);

  LOG_DEBUG("SDL_version: %d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);

  window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height,
                            SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    FATAL("SDL_CreateWindow() failed");
  }

  if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0) {
    LOG_ERROR("SDL_SetRelativeMouseMode() failed");
  }

  context = SDL_GL_CreateContext(window);

  if (context == nullptr) {
    FATAL("SDL_GL_CreateContext() failed");
  }

  sg_desc desc = {.logger = {.func = slog_func}};

  // desc.allocator = {
  //     .alloc = [](size_t size, [[maybe_unused]] void* user_data) { return allocator::_malloc(size); },
  //     .free = [](void* ptr, [[maybe_unused]] void* user_data) { allocator::_free(ptr); },
  // };

  sg_setup(desc);

  material::init();
  shapes::init();
  ui::init();
}

Mesh create_mesh(const std::span<Vertex> vertex_data, const std::span<index_t> index_data) {
  const sg_buffer_desc vertex_buffer_desc = {.data = {
                                                 .ptr = vertex_data.data(),
                                                 .size = vertex_data.size() * sizeof(Vertex),
                                             }};

  sg_buffer vertex_buffer = sg_make_buffer(vertex_buffer_desc);

  const sg_buffer_desc index_buffer_desc = {
      .type = SG_BUFFERTYPE_INDEXBUFFER,
      .data =
          {
              .ptr = index_data.data(),
              .size = index_data.size() * sizeof(index_t),
          },
  };

  sg_buffer index_buffer = sg_make_buffer(index_buffer_desc);

  return {
      .bindings = {.vertex_buffers = {vertex_buffer}, .index_buffer = index_buffer},
      .num_elements = static_cast<u32>(index_data.size()),
  };
}

void begin_frame(entt::entity camera) {
  const sg_pass_action pass_action = {
      .colors = {{
          .action = SG_ACTION_CLEAR,
          .value = SG_DARK_GRAY,
      }},
  };

  sg_begin_default_pass(&pass_action, window_width, window_height);

  // camera.transform.update();

  const comp::Transform& camera_transform = world::registry->get<comp::Transform>(camera);
  comp::Camera& camera_component = world::registry->get<comp::Camera>(camera);

  if (camera_component.width != window_width || camera_component.height != window_height) {
    LOG_DEBUG("camera_component.update()");
    camera_component.width = window_width;
    camera_component.height = window_height;
    camera_component.update();
  }

  current_vp = camera_component.projection * glm::inverse(camera_transform.world);
}

void draw_world() {
  for (const auto [entity, transform, renderable] : world::registry->view<comp::Transform, comp::Renderable>().each()) {
    const mat4 mvp = current_vp * transform.world;

    sg_apply_pipeline(renderable.material.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(mvp));

    sg_apply_bindings(renderable.mesh.bindings);
    sg_draw(renderable.mesh.base_element, renderable.mesh.num_elements, 1);
  }
}

void end_frame() {
  sg_end_pass();
  sg_commit();
}

void present() { SDL_GL_SwapWindow(window); }

void finish() {
  ui::finish();
  shapes::finish();
  material::finish();

  sg_shutdown();

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);

  SDL_Quit();

  LOG_INFO("gfx::finish()");
}

ivec2 get_width_height() { return {window_width, window_height}; }

SDL_Window* get_sdl_window() { return window; }

const mat4& get_current_vp() { return current_vp; }

}  // namespace gfx
