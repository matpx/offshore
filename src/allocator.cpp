#include "allocator.hpp"

#include <malloc.h>

#include <cstring>
#include <cassert>

#include "types.hpp"

namespace allocator {

static i64 alloc_count = 0;

void* _malloc(size_t size) {
  alloc_count++;
  return malloc(size);
}

void* _aligned_alloc(size_t alignment, size_t size) {
  alloc_count++;
  return aligned_alloc(alignment, size);
}

void* _realloc(void* ptr, size_t size) {
  if (size == 0 && ptr != nullptr) {
    alloc_count--;
  } else if (ptr == nullptr) {
    alloc_count++;
  }

  return realloc(ptr, size);
}

void* _aligned_realloc(size_t alignment, void* ptr, size_t size) {
  if (size == 0 && ptr != nullptr) {
    alloc_count--;
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
  alloc_count--;
  free(ptr);
}

void debug_leak_check() {
  assert(alloc_count == 0);
}

static_assert(std::alignment_of<std::max_align_t>() >= base_alignment);

}  // namespace allocator
