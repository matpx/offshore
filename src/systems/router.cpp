#include "router.hpp"

#include "player.hpp"
#include "terrain.hpp"

namespace systems::router {

static Player player;
static Terrain terrain;

void setup() {
  player.setup();
  terrain.setup();
}

void frame(double delta_time) { player.update(delta_time); }

void finish() {}

}  // namespace router
