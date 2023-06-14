#pragma once

#include "../core/types.hpp"

namespace gfx::shapes {

void init();

void begin_pass();

void draw_sphere(const vec3& position, const vec3& scale = {1, 1, 1});

void draw_box(const vec3& position, const vec3& scale = {1, 1, 1});

void draw_aabb(const AABB& aabb);

void draw_all_aabb();

void finish_pass();

void finish();

}  // namespace gfx::shapes