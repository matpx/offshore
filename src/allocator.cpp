#include "allocator.hpp"

#include <malloc.h>

#include "types.hpp"

#ifndef NDEBUG
#define STB_LEAKCHECK_IMPLEMENTATION
// #define STB_LEAKCHECK_SHOWALL
#include <stb/stb_leakcheck.h>
#endif

namespace allocator {

void* _malloc(size_t size) { return malloc(size); }

void* _aligned_alloc(size_t alignment, size_t size) { return aligned_alloc(alignment, size); }

void* _realloc(void* ptr, size_t size) { return realloc(ptr, size); }

void* _aligned_realloc(size_t alignment, void* ptr, size_t size) {
  if (size == 0 && ptr != nullptr) {
    free(ptr);
  }

  void* aligned_ptr = _aligned_alloc(alignment, size);

  if (ptr == nullptr) {
    return aligned_ptr;
  }

  size_t copy_size = std::min(size, malloc_usable_size(ptr));

  memcpy(aligned_ptr, ptr, copy_size);

  free(ptr);

  return aligned_ptr;
}

void _free(void* ptr) { free(ptr); }

void debug_leak_check() {
#ifndef NDEBUG
  stb_leakcheck_dumpmem();
#endif
}

static_assert(std::alignment_of<std::max_align_t>() >= base_alignment);

}  // namespace allocator
