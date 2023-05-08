#pragma once

#include <cstddef>

namespace allocator {

constexpr size_t base_alignment = 16;

void* _malloc(size_t size);
void* _aligned_alloc(size_t alignment, size_t size);
void* _realloc(void* ptr, size_t size);
void _free(void* ptr);
void debug_leak_check();

}
