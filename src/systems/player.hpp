#pragma once

#include "system.hpp"

namespace systems {

struct Player : System {
  void setup();

  void update(double delta_time);

  void finish() {}
};

}  // namespace systems
