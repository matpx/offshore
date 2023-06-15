#pragma once

#include "system.hpp"

namespace systems {

struct Player final : System {
  void setup() override;

  void update(double delta_time) override;
};

}  // namespace systems
