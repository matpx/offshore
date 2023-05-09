#include "allocator.hpp"

#include <malloc.h>

#include <cstring>
#include <cassert>

#include "types.hpp"

namespace allocator {

#ifndef NDEBUG
static i64 alloc_count = 0;
inline void increase_alloc_count() {alloc_count++;}
inline void decrease_alloc_count() {alloc_count--;}
#elif
inline void increase_alloc_count() {}
inline void decrease_alloc_count() {}
#endif

void* _malloc(size_t size) {
  increase_alloc_count();
  return malloc(size);
}

void* _aligned_alloc(size_t alignment, size_t size) {
  increase_alloc_count();
  return aligned_alloc(alignment, size);
}

void* _realloc(void* ptr, size_t size) {
  if (size == 0 && ptr != nullptr) {
    decrease_alloc_count();
  } else if (ptr == nullptr) {
    increase_alloc_count();
  }

  return realloc(ptr, size);
}

void* _aligned_realloc(size_t alignment, void* ptr, size_t size) {
  if (size == 0 && ptr != nullptr) {
    decrease_alloc_count();
    free(ptr);
    return nullptr;
  }

  void* aligned_ptr = _aligned_alloc(alignment, size);

  if (ptr == nullptr) {
    return aligned_ptr;
  }

  size_t copy_size = std::min(size, malloc_usable_size(ptr));

  memcpy(aligned_ptr, ptr, copy_size);

  _free(ptr);

  return aligned_ptr;
}

void _free(void* ptr) {
  decrease_alloc_count();
  free(ptr);
}

void debug_leak_check() {
  assert(alloc_count == 0);
}

}  // namespace allocator
