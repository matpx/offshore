#pragma once

#include "system.hpp"
namespace systems {

struct Terrain final : System {
  void setup();

  void update([[maybe_unused]] double delta_time) {}

  void finish() {}
};

}  // namespace systems
