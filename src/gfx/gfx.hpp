#pragma once

#include <SDL2/SDL_video.h>

#include "../world/world.hpp"

namespace gfx {

void init();

Mesh create_mesh(const container::Span<Vertex> vertex_data, const container::Span<index_t> index_data);

void begin_frame(world::Entity& camera);

void draw_world();

void draw_sphere();

void end_frame();

void present();

void finish();

extern mat4 current_vp;

}  // namespace gfx
