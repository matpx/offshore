#pragma once

#include "system.hpp"
namespace systems {

struct Terrain final : System {
  void update(double delta_time) override;
};

}  // namespace systems
