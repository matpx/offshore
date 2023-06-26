#include "shapes.hpp"

// #include <sokol/sokol_gfx.h>

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#include <array>
#include <cassert>
#include <entt/entity/registry.hpp>
#include <vector>

#include "../core/log.hpp"
#include "debug_main_ps.spirv.h"
#include "debug_main_vs.spirv.h"
#include "device.hpp"
#include "gfx.hpp"
#include "vertex.hpp"
#include "window.hpp"

namespace gfx::shapes {

const static std::array<SimpleVertex, 24> cube_vertex_data = {
    SimpleVertex{{-0.5, -0.5f, -0.5f, 1.0f}}, SimpleVertex{{0.5, -0.5f, -0.5f, 1.0f}},
    SimpleVertex{{0.5, -0.5f, -0.5f, 1.0f}},  SimpleVertex{{0.5, 0.5f, -0.5f, 1.0f}},
    SimpleVertex{{0.5, 0.5f, -0.5f, 1.0f}},   SimpleVertex{{-0.5, 0.5f, -0.5f, 1.0f}},
    SimpleVertex{{-0.5, 0.5f, -0.5f, 1.0f}},  SimpleVertex{{-0.5, -0.5f, -0.5f, 1.0f}},
    SimpleVertex{{-0.5, -0.5f, 0.5f, 1.0f}},  SimpleVertex{{0.5, -0.5f, 0.5f, 1.0f}},
    SimpleVertex{{0.5, -0.5f, 0.5f, 1.0f}},   SimpleVertex{{0.5, 0.5f, 0.5f, 1.0f}},
    SimpleVertex{{0.5, 0.5f, 0.5f, 1.0f}},    SimpleVertex{{-0.5, 0.5f, 0.5f, 1.0f}},
    SimpleVertex{{-0.5, 0.5f, 0.5f, 1.0f}},   SimpleVertex{{-0.5, -0.5f, 0.5f, 1.0f}},
    SimpleVertex{{-0.5, -0.5f, -0.5f, 1.0f}}, SimpleVertex{{-0.5, -0.5f, 0.5f, 1.0f}},
    SimpleVertex{{0.5, -0.5f, -0.5f, 1.0f}},  SimpleVertex{{0.5, -0.5f, 0.5f, 1.0f}},
    SimpleVertex{{0.5, 0.5f, -0.5f, 1.0f}},   SimpleVertex{{0.5, 0.5f, 0.5f, 1.0f}},
    SimpleVertex{{-0.5, 0.5f, -0.5f, 1.0f}},  SimpleVertex{{-0.5, 0.5f, 0.5f, 1.0f}},
};

static bool shapes_pass_active = false;

static nvrhi::GraphicsPipelineHandle graphics_pipeline = nullptr;
static nvrhi::BufferHandle cube_vertex_buffer = nullptr;
static nvrhi::BufferHandle constant_buffer = nullptr;
static nvrhi::BindingSetHandle cube_binding_set = nullptr;

static void init_pipeline() {
  LOG_DEBUG("gfx::shapes::init_pipeline()");

  nvrhi::ShaderDesc vertex_shader_desc(nvrhi::ShaderType::Vertex);
  vertex_shader_desc.entryName = "main_vs";

  nvrhi::ShaderHandle vertex_shader =
      device::get_device()->createShader(vertex_shader_desc, g_debug_main_vs_spirv, sizeof(g_debug_main_vs_spirv));

  const nvrhi::VertexAttributeDesc attributes[] = {
      nvrhi::VertexAttributeDesc()
          .setName("POSITION")
          .setFormat(nvrhi::Format::RGB32_FLOAT)
          .setOffset(offsetof(SimpleVertex, position))
          .setElementStride(sizeof(SimpleVertex)),
  };

  const nvrhi::InputLayoutHandle input_layout =
      device::get_device()->createInputLayout(attributes, uint32_t(std::size(attributes)), vertex_shader);

  nvrhi::ShaderDesc pixel_shader_desc(nvrhi::ShaderType::Pixel);
  pixel_shader_desc.entryName = "main_ps";

  nvrhi::ShaderHandle pixel_shader =
      device::get_device()->createShader(pixel_shader_desc, g_debug_main_ps_spirv, sizeof(g_debug_main_ps_spirv));

  const auto layout_desc = nvrhi::BindingLayoutDesc()
                               .setVisibility(nvrhi::ShaderType::All)
                               .addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0));

  nvrhi::BindingLayoutHandle binding_layout = device::get_device()->createBindingLayout(layout_desc);

  const auto pipeline_desc =
      nvrhi::GraphicsPipelineDesc()
          .addBindingLayout(binding_layout)
          .setInputLayout(input_layout)
          .setVertexShader(vertex_shader)
          .setPixelShader(pixel_shader)
          .setPrimType(nvrhi::PrimitiveType::LineList);

  graphics_pipeline = device::get_device()->createGraphicsPipeline(pipeline_desc, device::get_current_framebuffer());
}

static void init_buffer() {
  LOG_DEBUG("gfx::shapes::init_buffer()");

  const auto constant_buffer_desc = nvrhi::BufferDesc()
                                        .setByteSize(sizeof(mat4))
                                        .setIsConstantBuffer(true)
                                        .setIsVolatile(true)
                                        .setMaxVersions(256);  // TODO too low?

  constant_buffer = device::get_device()->createBuffer(constant_buffer_desc);

  const auto vertex_buffer_desc = nvrhi::BufferDesc()
                                      .setByteSize(sizeof(cube_vertex_data))
                                      .setIsVertexBuffer(true)
                                      .setInitialState(nvrhi::ResourceStates::VertexBuffer)
                                      .setKeepInitialState(true)
                                      .setDebugName("Vertex Buffer");

  cube_vertex_buffer = device::get_device()->createBuffer(vertex_buffer_desc);

  const auto binding_set_desc =
      nvrhi::BindingSetDesc().addItem(nvrhi::BindingSetItem::ConstantBuffer(0, constant_buffer));

  cube_binding_set =
      device::get_device()->createBindingSet(binding_set_desc, graphics_pipeline->getDesc().bindingLayouts[0]);

  nvrhi::CommandListHandle upload_command_list = device::get_device()->createCommandList();

  upload_command_list->open();
  upload_command_list->writeBuffer(cube_vertex_buffer, cube_vertex_data.data(), sizeof(cube_vertex_data));
  upload_command_list->close();

  device::get_device()->executeCommandList(upload_command_list);
}

void init() {
  LOG_INFO("gfx::shapes::init()");

  init_pipeline();
  init_buffer();
}

void begin_pass() {
  assert(!shapes_pass_active);

  // sg_apply_pipeline(unlit_pipeline);
  // sg_apply_bindings(cube_bindings);

  shapes_pass_active = true;
}

static void draw_shape(const u32 base_element, const u32 num_elements, const vec3& center, const vec3& scale) {
  assert(shapes_pass_active);

  const mat4 mvp = gfx::get_current_vp() * glm::scale(glm::translate(glm::identity<mat4>(), center), scale);

  nvrhi::CommandListHandle draw_command_list = device::get_device()->createCommandList();

  draw_command_list->open();

  draw_command_list->writeBuffer(constant_buffer, glm::value_ptr(mvp), sizeof(mvp));

  const auto vertex_buffer_binding = nvrhi::VertexBufferBinding{
      .buffer = cube_vertex_buffer,
      .offset = base_element,
  };

  const auto graphicsState = nvrhi::GraphicsState()
                                 .setPipeline(graphics_pipeline)
                                 .setFramebuffer(device::get_current_framebuffer())
                                 .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(
                                     nvrhi::Viewport(window::get_width_height().x, window::get_width_height().y)))
                                 .addBindingSet(cube_binding_set)
                                 .addVertexBuffer(vertex_buffer_binding);

  draw_command_list->setGraphicsState(graphicsState);

  const auto drawArguments = nvrhi::DrawArguments().setVertexCount(num_elements);
  draw_command_list->draw(drawArguments);

  draw_command_list->close();
  device::get_device()->executeCommandList(draw_command_list);
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

  cube_binding_set = nullptr;
  cube_vertex_buffer = nullptr;
  constant_buffer = nullptr;
  graphics_pipeline = nullptr;

  LOG_INFO("gfx::shapes::finish()");
}

}  // namespace gfx::shapes