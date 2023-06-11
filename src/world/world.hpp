#pragma once

#include <entt/fwd.hpp>

#include "../core/container.hpp"
#include "components.hpp"

namespace world {

void init();

void update();

extern std::unique_ptr<entt::registry> registry;

extern entt::entity main_camera;

}  // namespace world
