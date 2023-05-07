#define STB_LEAKCHECK_IMPLEMENTATION
// #define STB_LEAKCHECK_SHOWALL
#include <stb/stb_leakcheck.h>

#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>

#include <type_traits>
static_assert(std::alignment_of<stb_leakcheck_malloc_info>() == 16);
static_assert(std::alignment_of<stbds_array_header>() == 16);
