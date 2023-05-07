#pragma once

#include "allocator.hpp"
#include "log.hpp"
#include "types.hpp"

#define STBDS_NO_SHORT_NAMES
#define STBDS_REALLOC(context, ptr, size) allocator::_realloc(ptr, size)
#define STBDS_FREE(context, ptr) allocator::_free(ptr)
#include <stb/stb_ds.h>

namespace utils {

template <typename T>
struct Vector {
  T *_data = nullptr;

  constexpr size_t size() const { return stbds_arrlenu(_data); }
  constexpr size_t set_size(size_t size) { return stbds_arrsetlen(_data, size); }

  constexpr T pop() { return stbds_arrpop(_data); }

  constexpr T push(T &value) { return stbds_arrpush(_data, value); }
  constexpr T push(const T &value) { return stbds_arrpush(_data, value); }

  constexpr void clear() { stbds_arrfree(_data); }

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

}  // namespace utils
