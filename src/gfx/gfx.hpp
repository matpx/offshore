#pragma once

#include <span>

#include "../world/world.hpp"

namespace gfx {

void init();

Mesh create_mesh(const std::span<const Vertex> vertex_data, const std::span<const index_t> index_data);

void destroy_mesh(const Mesh& mesh);

void begin_frame(entt::entity camera);

void draw_world();

void draw_sphere();

void end_frame();

void finish();

const mat4& get_current_vp();

}  // namespace gfx
