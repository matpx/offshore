#pragma once

namespace systems {

struct System {
  virtual void setup(){};
  virtual void update([[maybe_unused]] double delta_time){};
  virtual void finish(){};
};

}  // namespace systems
