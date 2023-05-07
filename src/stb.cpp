#include "utils.hpp"

#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>

static_assert(std::alignment_of<std::max_align_t>() >= std::alignment_of<stbds_array_header>());
