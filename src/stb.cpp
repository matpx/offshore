#define STB_LEAKCHECK_IMPLEMENTATION
// #define STB_LEAKCHECK_SHOWALL
#include <stb/stb_leakcheck.h>

#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>

#include <cstddef>
#include <type_traits>
static_assert(std::alignment_of<std::max_align_t>() >= std::alignment_of<stb_leakcheck_malloc_info>());
static_assert(std::alignment_of<std::max_align_t>() >= std::alignment_of<stbds_array_header>());
