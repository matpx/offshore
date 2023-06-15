#pragma once

#include "system.hpp"
namespace systems {

struct Terrain final : System {
  void setup() override;
};

}  // namespace systems
