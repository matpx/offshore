#include "passes.hpp"

#include <entt/entity/registry.hpp>

#include "device.hpp"
#include "gfx.hpp"

namespace gfx::passes {

static nvrhi::CommandListHandle command_list;

void init() { command_list = device::get_device()->createCommandList(); }

void draw_world() {
  const uvec2 viewport_size = device::get_current_viewport();

  nvrhi::FramebufferHandle current_framebuffer = device::get_current_framebuffer();

  command_list->open();

  for (const auto [entity, transform, renderable] :
       world::registry->view<const comp::Transform, const comp::Renderable>().each()) {
    const mat4 mvp = gfx::get_current_vp() * transform.world;

    command_list->writeBuffer(renderable.material.constant_buffer, glm::value_ptr(mvp), sizeof(mvp));

    const auto graphicsState = nvrhi::GraphicsState()
                             .setPipeline(renderable.material.pipeline)
                             .setFramebuffer(current_framebuffer)
                             .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(
                                 nvrhi::Viewport(viewport_size.x, viewport_size.y)))
                             .addBindingSet(renderable.material.binding_set)
                             .addVertexBuffer({.buffer = renderable.mesh.vertex_buffer})
                             .setIndexBuffer({.buffer = renderable.mesh.index_buffer});

    command_list->setGraphicsState(graphicsState);

    const auto drawArguments = nvrhi::DrawArguments()
                             .setStartIndexLocation(renderable.mesh.base_element)
                             .setVertexCount(renderable.mesh.num_elements);
    command_list->drawIndexed(drawArguments);
  }

  command_list->close();
  device::get_device()->executeCommandList(command_list);
}

void finish() { command_list = nullptr; }

}  // namespace gfx::passes
