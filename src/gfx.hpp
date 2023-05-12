#pragma once

#include <SDL2/SDL_video.h>

#include "world.hpp"

namespace gfx {

void init();

Mesh create_mesh(const utils::Span<Vertex> vertex_data, const utils::Span<index_t> index_data);

void begin_frame(world::Entity& camera);

void draw_world();

void draw_sphere();

void end_frame();

void present();

void finish();

}  // namespace gfx
