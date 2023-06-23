#include "shapes.hpp"

// #include <sokol/sokol_gfx.h>

#include <nvrhi/nvrhi.h>

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

static bool shapes_pass_active = false;

static nvrhi::GraphicsPipelineHandle graphics_pipeline = nullptr;

static void init_pipeline() {
  LOG_DEBUG("gfx::shapes::init_pipeline()");

  nvrhi::ShaderHandle vertex_shader = device::get_device()->createShader(
      nvrhi::ShaderDesc(nvrhi::ShaderType::Vertex), g_debug_main_vs_spirv, sizeof(g_debug_main_vs_spirv));

  const nvrhi::VertexAttributeDesc attributes[] = {
      nvrhi::VertexAttributeDesc()
          .setName("POSITION")
          .setFormat(nvrhi::Format::RGB32_FLOAT)
          .setOffset(offsetof(SimpleVertex, position))
          .setElementStride(sizeof(SimpleVertex)),
  };

  const nvrhi::InputLayoutHandle inputLayout =
      device::get_device()->createInputLayout(attributes, uint32_t(std::size(attributes)), vertex_shader);

  nvrhi::ShaderHandle pixel_shader = device::get_device()->createShader(
      nvrhi::ShaderDesc(nvrhi::ShaderType::Pixel), g_debug_main_ps_spirv, sizeof(g_debug_main_ps_spirv));

  const auto layoutDesc = nvrhi::BindingLayoutDesc()
                              .setVisibility(nvrhi::ShaderType::All)
                              .addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))
                              .addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0));

  const nvrhi::BindingLayoutHandle binding_layout = device::get_device()->createBindingLayout(layoutDesc);

  const auto pipeline_desc = nvrhi::GraphicsPipelineDesc()
                                 .setInputLayout(inputLayout)
                                 .setVertexShader(vertex_shader)
                                 .setPixelShader(pixel_shader)
                                 .addBindingLayout(binding_layout);

  graphics_pipeline = device::get_device()->createGraphicsPipeline(pipeline_desc, device::get_framebuffers()[0]);
}

static void init_buffer() {
  LOG_DEBUG("gfx::shapes::init_buffer()");

  const auto constant_buffer_desc = nvrhi::BufferDesc()
                                        .setByteSize(sizeof(mat4))
                                        .setIsConstantBuffer(true)
                                        .setIsVolatile(true)
                                        .setMaxVersions(16);  // TODO too low?

  nvrhi::BufferHandle constant_buffer = device::get_device()->createBuffer(constant_buffer_desc);

  const auto vertex_buffer_desc = nvrhi::BufferDesc()
                                      .setByteSize(sizeof(cube_vertex_data))
                                      .setIsVertexBuffer(true)
                                      .setInitialState(nvrhi::ResourceStates::VertexBuffer)
                                      .setKeepInitialState(true)
                                      .setDebugName("Vertex Buffer");

  nvrhi::BufferHandle vertex_buffer = device::get_device()->createBuffer(vertex_buffer_desc);

  const auto binding_set_desc =
      nvrhi::BindingSetDesc().addItem(nvrhi::BindingSetItem::ConstantBuffer(0, constant_buffer));

  nvrhi::BindingSetHandle binding_set =
      device::get_device()->createBindingSet(binding_set_desc, graphics_pipeline->getDesc().bindingLayouts[0]);

  // commandList->open();

  // commandList->writeBuffer(vertexBuffer, g_Vertices, sizeof(g_Vertices));

  // const void* textureData = ...;
  // const size_t textureRowPitch = ...;
  // commandList->writeTexture(geometryTexture,
  //     /* arraySlice = */ 0, /* mipLevel = */ 0,
  //     textureData, textureRowPitch);

  // commandList->close();
  // device::get_device()->executeCommandList(commandList);
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

static void draw_shape(i32 base_element, i32 num_elements, const vec3& center, const vec3& scale) {
  assert(shapes_pass_active);

  const mat4 mvp = gfx::get_current_vp() * glm::scale(glm::translate(glm::identity<mat4>(), center), scale);

  // MVP_t MVP = {
  //     .mvp = mvp,
  // };
  // sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_MVP, SG_RANGE(MVP));

  // sg_draw(base_element, num_elements, 1);
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

  // sg_destroy_buffer(cube_bindings.vertex_buffers[0]);

  // sg_destroy_pipeline(unlit_pipeline);
  // sg_destroy_shader(unlit_shader);

  LOG_INFO("gfx::shapes::finish()");
}

}  // namespace gfx::shapes