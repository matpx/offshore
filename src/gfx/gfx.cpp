#include "gfx.hpp"

#include <SDL2/SDL.h>
#include <nvrhi/nvrhi.h>

#include <entt/entity/registry.hpp>

#include "../core/geometry.hpp"
#include "../core/log.hpp"
#include "device.hpp"
#include "material.hpp"
#include "passes.hpp"
#include "renderable.hpp"
#include "shapes.hpp"
#include "ui.hpp"
#include "vertex.hpp"
#include "window.hpp"

namespace gfx {

static_assert(sizeof(index_t) == 4);

static mat4 current_vp = mat4(1.0);

void init() {
  LOG_INFO("gfx::init()");

  window::init();

  device::init();
  material::init();
  passes::init();
  shapes::init();
  ui::init();
}

Mesh create_mesh(const std::span<const Vertex> vertex_data, const std::span<const index_t> index_data) {
  geometry::AABB aabb = geometry::AABB::from_vertex_data(vertex_data);

  const auto vertex_buffer_desc = nvrhi::BufferDesc()
                                      .setByteSize(vertex_data.size_bytes())
                                      .setIsVertexBuffer(true)
                                      .setInitialState(nvrhi::ResourceStates::VertexBuffer)
                                      .setKeepInitialState(true)
                                      .setDebugName("Vertex Buffer");

  const auto vertex_buffer = device::get_device()->createBuffer(vertex_buffer_desc);

  const auto index_buffer_desc = nvrhi::BufferDesc()
                                     .setByteSize(index_data.size_bytes())
                                     .setIsIndexBuffer(true)
                                     .setInitialState(nvrhi::ResourceStates::IndexBuffer)
                                     .setKeepInitialState(true)
                                     .setDebugName("Index Buffer");

  const auto index_buffer = device::get_device()->createBuffer(index_buffer_desc);

  nvrhi::CommandListHandle upload_command_list = device::get_device()->createCommandList();

  upload_command_list->open();
  upload_command_list->writeBuffer(vertex_buffer, vertex_data.data(), vertex_data.size_bytes());
  upload_command_list->writeBuffer(index_buffer, index_data.data(), index_data.size_bytes());
  upload_command_list->close();

  device::get_device()->executeCommandList(upload_command_list);

  return {
      .vertex_buffer = vertex_buffer,
      .index_buffer = index_buffer,
      .num_elements = static_cast<u32>(index_data.size()),
      .local_aabb = aabb,
  };
}

void begin_frame(entt::entity camera) {
  const uvec2 window_size = window::get_width_height();

  const comp::Transform& camera_transform = world::registry->get<comp::Transform>(camera);
  comp::Camera& camera_component = world::registry->get<comp::Camera>(camera);

  if (camera_component.width != window_size.x || camera_component.height != window_size.y) {
    LOG_DEBUG("camera_component.update()");
    camera_component.width = window_size.x;
    camera_component.height = window_size.y;
    camera_component.update();
  }

  current_vp = camera_component.projection * glm::inverse(camera_transform.world);

  device::begin_frame();
}

void draw_all() { passes::draw_world(); }

void end_frame() { device::finish_frame(); }

void finish() {
  device::wait_idle();

  ui::finish();
  shapes::finish();
  passes::finish();
  material::finish();
  device::finish();

  window::finish();

  LOG_INFO("gfx::finish()");
}

const mat4& get_current_vp() { return current_vp; }

}  // namespace gfx
