#include "router.hpp"

#include "player.hpp"
#include "terrain.hpp"

namespace systems::router {

static Player player;
static Terrain terrain;

void setup() {
  // terrain.setup();
  player.setup();
}

void update(double delta_time) {
  // terrain.update(delta_time);
  player.update(delta_time);
}

void finish() {}

}  // namespace systems::router
