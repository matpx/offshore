#pragma once

namespace systems {

struct System {
  virtual void setup() = 0;
  virtual void update(double delta_time) = 0;
  virtual void finish() = 0;
};

}  // namespace systems
