#include "world.hpp"

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "../core/log.hpp"
#include "components.hpp"

namespace world {

std::unique_ptr<entt::registry> registry;

entt::entity main_camera = entt::null;

AABB transform_aabb(const AABB& aabb, const mat4 transform_matrix) { 
  // https://zeux.io/2010/10/17/aabb-from-obb-with-component-wise-abs/
  
  mat4 rotation_mat = (mat3)transform_matrix;

  float* rotation_mat_ptr = glm::value_ptr(rotation_mat);

  for (i32 i = 0; i < 4 * 4; i++) {
    rotation_mat_ptr[i] = std::abs(rotation_mat_ptr[i]);
  }

  const vec3 new_center = transform_matrix * vec4(aabb.center, 1.0f);
  const vec3 new_extent = rotation_mat * vec4(aabb.extent, 1.0f);

  return {new_center, new_extent};
}

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
      transform.global_aabb = transform_aabb(renderable->mesh.local_aabb, transform.world);
    }
  }
}

}  // namespace world
