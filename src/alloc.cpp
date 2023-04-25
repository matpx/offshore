#include "alloc.hpp"
#include "defines.hpp"
#include <cassert>
#include <malloc.h>

static i64 alloc_count = 0;

void *omalloc(size_t n) {
  #ifndef NDEBUG
  alloc_count++;
  #endif

  return malloc(n);
}

void ofree(void* ptr) {
  #ifndef NDEBUG
  alloc_count--;
  #endif

  free(ptr);
}

void debug_leak_check() {
  assert(alloc_count == 0);
}

