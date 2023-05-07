#include "allocator.hpp"

#include "types.hpp"

#ifndef NDEBUG
#define STB_LEAKCHECK_IMPLEMENTATION
// #define STB_LEAKCHECK_SHOWALL
#include <stb/stb_leakcheck.h>
#endif

namespace allocator {

void* _malloc(size_t size) { return malloc(size); }

void* _realloc(void* ptr, size_t size) { return realloc(ptr, size); }

void _free(void* ptr) { free(ptr); }

void debug_leak_check() {
#ifndef NDEBUG
  stb_leakcheck_dumpmem();
#endif
}

static_assert(std::alignment_of<std::max_align_t>() >= alignment);

}  // namespace allocator
