#include "world.hpp"

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "../core/log.hpp"
#include "components.hpp"

namespace world {

std::unique_ptr<entt::registry> registry;

entt::entity main_camera = entt::null;

void init() { registry = std::make_unique<entt::registry>(); }

void update() {
  for (auto [entity, transform] : world::registry->view<comp::Transform>().each()) {
    if (transform.parent_id != entt::null) {
      const comp::Transform& parent_transform = world::registry->get<comp::Transform>(transform.parent_id);

      transform.world = glm::translate(glm::identity<mat4>(), transform.translation) * glm::toMat4(transform.rotation);
      transform.world = parent_transform.world * transform.world;
    } else {
      transform.world = glm::translate(glm::identity<mat4>(), transform.translation) * glm::toMat4(transform.rotation);
    }

    comp::Renderable* renderable = world::registry->try_get<comp::Renderable>(entity);

    if (renderable) {
      transform.global_aabb = geometry::transform_aabb(renderable->mesh.local_aabb, transform.world);
    }
  }
}

}  // namespace world
