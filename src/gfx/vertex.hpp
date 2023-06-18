#pragma once

#include "../core/types.hpp"

namespace gfx {

using index_t = u16;

struct Vertex {
    float positions[3];
    float normal[3];
};

struct SimpleVertex {
    float positions[3];
};

}