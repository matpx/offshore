#pragma once

#include "../core/types.hpp"

namespace gfx {

using index_t = u32;

struct Vertex {
    float position[4];
    float normal[4];
};

struct SimpleVertex {
    float position[4];
};

}