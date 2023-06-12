#pragma once

#include <SDL2/SDL_video.h>
#include <entt/entity/fwd.hpp>
#include <span>

#include "../world/world.hpp"

namespace gfx {

void init();

Mesh create_mesh(const std::span<Vertex> vertex_data, const std::span<index_t> index_data);

void begin_frame(entt::entity camera);

void draw_world();

void draw_sphere();

void end_frame();

void present();

void finish();

ivec2 get_width_height();
SDL_Window* get_sdl_window();
const mat4& get_current_vp(); 

}  // namespace gfx
