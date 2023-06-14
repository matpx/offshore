#include "gfx.hpp"

#include <SDL2/SDL.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/util/sokol_color.h>

#include <cassert>
#include <entt/entity/registry.hpp>

#include "../core/log.hpp"
#include "material.hpp"
#include "renderable.hpp"
#include "shapes.hpp"
#include "ui.hpp"
#include "window.hpp"

namespace gfx {

static_assert(sizeof(index_t) == 2);

static SDL_GLContext context = nullptr;

static mat4 current_vp = mat4(1.0);

void init() {
  LOG_INFO("gfx::init()");

  window::init();

  assert(context == nullptr);

  context = SDL_GL_CreateContext(window::get_sdl_window());

  if (context == nullptr) {
    FATAL("SDL_GL_CreateContext() failed");
  }

  sg_desc desc = {
      .buffer_pool_size = 256,
      .logger = {.func = slog_func},
  };

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

  AABB aabb = {{0, 0, 0}, {0, 0, 0}};

  for (const Vertex& vertex : vertex_data) { // TODO: not always needed
    if (vertex.positions[0] < aabb.min.x) aabb.min.x = vertex.positions[0];
    if (vertex.positions[1] < aabb.min.y) aabb.min.y = vertex.positions[1];
    if (vertex.positions[2] < aabb.min.z) aabb.min.z = vertex.positions[2];

    if (vertex.positions[0] > aabb.max.x) aabb.max.x = vertex.positions[0];
    if (vertex.positions[1] > aabb.max.y) aabb.max.y = vertex.positions[1];
    if (vertex.positions[2] > aabb.max.z) aabb.max.z = vertex.positions[2];
  }

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
      .local_aabb = aabb,
  };
}

void begin_frame(entt::entity camera) {
  const sg_pass_action pass_action = {
      .colors = {{
          .action = SG_ACTION_CLEAR,
          .value = SG_DARK_GRAY,
      }},
  };

  const uvec2 window_size = window::get_width_height();

  sg_begin_default_pass(&pass_action, window_size.x, window_size.y);

  // camera.transform.update();

  const comp::Transform& camera_transform = world::registry->get<comp::Transform>(camera);
  comp::Camera& camera_component = world::registry->get<comp::Camera>(camera);

  if (camera_component.width != window_size.x || camera_component.height != window_size.y) {
    LOG_DEBUG("camera_component.update()");
    camera_component.width = window_size.x;
    camera_component.height = window_size.y;
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

void present() { SDL_GL_SwapWindow(window::get_sdl_window()); }

void finish() {
  assert(context != nullptr);

  ui::finish();
  shapes::finish();
  material::finish();

  sg_shutdown();

  SDL_GL_DeleteContext(context);

  window::finish();

  LOG_INFO("gfx::finish()");
}

const mat4& get_current_vp() { return current_vp; }

}  // namespace gfx
