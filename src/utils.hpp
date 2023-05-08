#pragma once

#include "allocator.hpp"
#include "log.hpp"
#include "types.hpp"

// #define STBDS_NO_SHORT_NAMES
// #define STBDS_REALLOC(context, ptr, size) allocator::_realloc(ptr, size)
// #define STBDS_FREE(allocator::_free, ptr) allocator::_free(allocator::_free)
// #include <stb/stb_ds.h>

namespace utils {

template <typename T, bool aligned = false>
struct Vector {
  T *_data = nullptr;
  size_t _size = 0;

  constexpr bool empty() const { return _data == nullptr; }

  constexpr size_t size() const { return _size; }
  constexpr void set_size(size_t size) {
    if constexpr (aligned) {
      _data = (T *)allocator::_aligned_realloc(alignof(T), _data, size * sizeof(T));
    } else {
      _data = (T *)allocator::_realloc(_data, size * sizeof(T));
    }
    _size = size;
  }

  constexpr void clear() {
    if (_data == nullptr) {
      return;
    }
    allocator::_free(_data);
    _data = nullptr;
    _size = 0;
  }

  constexpr const T &operator[](size_t pos) const {
    assert(pos < size());
    return _data[pos];
  }
  constexpr T &operator[](size_t pos) {
    assert(pos < size());
    return _data[pos];
  }

  Vector() = default;
  Vector(size_t size) { set_size(size); };
  Vector(const Vector &) = delete;
  Vector(Vector &&) = delete;
};

/*
template <typename T>
struct Span {
  const T *const _data;
  const size_t _data_size;

  explicit Span(const T *const data, size_t data_size) : _data(data), _data_size(data_size) {}

  explicit Span(const Vector<T> &vector)
      : _data(vector._data), _data_size(stbds_arrlen(vector._data)) {}

  constexpr size_t size() const { return _data_size; }

  constexpr const T &operator[](size_t pos) const {
    assert(pos < _data_size);
    return _data[pos];
  }
};

template <typename KEY, typename VALUE>
struct Hashmap {
  struct Internal {
    KEY key;
    VALUE value;
  };

  Internal *_data = nullptr;

  constexpr bool has(KEY key) { return stbds_hmgetp_null(_data, key) != nullptr; }

  constexpr const VALUE &get_or_default(KEY key) { return stbds_hmget(_data, key); }
  constexpr VALUE *get_or_null(KEY key) {
    Internal *i = stbds_hmgetp_null(_data, key);
    return i != nullptr ? &(i->value) : nullptr;
  }

  constexpr VALUE put(KEY key, VALUE value) { return stbds_hmput(_data, key, value); }
  constexpr bool remove(KEY key) { return stbds_hmdel(_data, key); }
  constexpr void clear() { stbds_hmfree(_data); }

  constexpr VALUE &operator[](KEY key) {
    Internal *i = stbds_hmgetp_null(_data, key);
    assert(i != nullptr);
    return i->value;
  }

  Hashmap() = default;
  Hashmap(const Hashmap &) = delete;
  Hashmap(Hashmap &&) = delete;
};
*/

}  // namespace utils
