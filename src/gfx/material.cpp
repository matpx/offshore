#include "material.hpp"

#include <nvrhi/nvrhi.h>

#include "../core/log.hpp"
#include "device.hpp"
#include "renderable.hpp"
#include "unlit_main_ps.spirv.h"
#include "unlit_main_vs.spirv.h"

namespace gfx::material {

static nvrhi::GraphicsPipelineHandle basic_pipeline = nullptr;
static nvrhi::BufferHandle constant_buffer = nullptr;
static nvrhi::BindingSetHandle binding_set = nullptr;

void init() {
  LOG_INFO("gfx::material::init()");

  nvrhi::ShaderDesc vertex_shader_desc(nvrhi::ShaderType::Vertex);
  vertex_shader_desc.entryName = "main_vs";

  nvrhi::ShaderHandle vertex_shader =
      device::get_device()->createShader(vertex_shader_desc, g_unlit_main_vs_spirv, sizeof(g_unlit_main_vs_spirv));

  const nvrhi::VertexAttributeDesc attributes[] = {
      nvrhi::VertexAttributeDesc()
          .setName("POSITION")
          .setFormat(nvrhi::Format::RGB32_FLOAT)
          .setOffset(offsetof(Vertex, position))
          .setElementStride(sizeof(Vertex)),
      nvrhi::VertexAttributeDesc()
          .setName("NORMAL")
          .setFormat(nvrhi::Format::RGB32_FLOAT)
          .setOffset(offsetof(Vertex, normal))
          .setElementStride(sizeof(Vertex)),
  };

  const nvrhi::InputLayoutHandle input_layout =
      device::get_device()->createInputLayout(attributes, uint32_t(std::size(attributes)), vertex_shader);

  nvrhi::ShaderDesc pixel_shader_desc(nvrhi::ShaderType::Pixel);
  pixel_shader_desc.entryName = "main_ps";

  nvrhi::ShaderHandle pixel_shader =
      device::get_device()->createShader(pixel_shader_desc, g_unlit_main_ps_spirv, sizeof(g_unlit_main_ps_spirv));

  const auto layout_desc = nvrhi::BindingLayoutDesc()
                               .setVisibility(nvrhi::ShaderType::All)
                               .addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0));

  nvrhi::BindingLayoutHandle binding_layout = device::get_device()->createBindingLayout(layout_desc);

  const auto pipeline_desc = nvrhi::GraphicsPipelineDesc()
                                 .addBindingLayout(binding_layout)
                                 .setInputLayout(input_layout)
                                 .setVertexShader(vertex_shader)
                                 .setPixelShader(pixel_shader)
                                 .setRenderState({.rasterState = {
                                                      .cullMode = nvrhi::RasterCullMode::None, // TODO outside only
                                                  }});

  basic_pipeline = device::get_device()->createGraphicsPipeline(pipeline_desc, device::get_current_framebuffer());

  const auto constant_buffer_desc = nvrhi::BufferDesc()
                                        .setByteSize(sizeof(mat4))
                                        .setIsConstantBuffer(true)
                                        .setIsVolatile(true)
                                        .setMaxVersions(256);  // TODO to push constants

  constant_buffer = device::get_device()->createBuffer(constant_buffer_desc);

  const auto binding_set_desc =
      nvrhi::BindingSetDesc().addItem(nvrhi::BindingSetItem::ConstantBuffer(0, constant_buffer));

  binding_set = device::get_device()->createBindingSet(binding_set_desc, basic_pipeline->getDesc().bindingLayouts[0]);
}

Material get() { return Material{basic_pipeline, constant_buffer, binding_set}; }

void finish() {
  binding_set = nullptr;
  constant_buffer = nullptr;
  basic_pipeline = nullptr;

  LOG_INFO("gfx::material::finish()");
}

}  // namespace gfx::material