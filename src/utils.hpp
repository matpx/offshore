#pragma once

#include <initializer_list>
#include "allocator.hpp"
#include "log.hpp"
#include "types.hpp"

// #define STBDS_NO_SHORT_NAMES
// #define STBDS_REALLOC(context, ptr, size) allocator::_realloc(ptr, size)
// #define STBDS_FREE(allocator::_free, ptr) allocator::_free(allocator::_free)
// #include <stb/stb_ds.h>

namespace utils {

template <typename T>
struct Vector {
 private:
  T *_data = nullptr;

  size_t _capacity = 0;
  size_t _size = 0;

  void ensure_capacity(size_t required_capacity) {
    if (required_capacity <= _capacity) {
      return;
    }

    if constexpr (std::alignment_of<T>() > std::alignment_of<max_align_t>()) {
      _data = (T *)allocator::_aligned_realloc(alignof(T), _data, required_capacity * sizeof(T));
    } else {
      _data = (T *)allocator::_realloc(_data, required_capacity * sizeof(T));
    }

    if (_data == nullptr) {
      FATAL("allocation failure!");
    }

    _capacity = required_capacity;
  }

 public:
  constexpr static size_t min_push_reserve = 4;

  constexpr size_t size() const { return _size; }
  constexpr size_t capacity() const { return _capacity; }

  constexpr void resize(size_t new_size) {
    ensure_capacity(new_size);

    _size = new_size;
  }

  constexpr void reserve(size_t new_capacity) {
    ensure_capacity(new_capacity);
  }

  constexpr void push(const T &value) {
    if (_size == _capacity) {
      ensure_capacity(min_push_reserve + _capacity * 2);
    }

    _data[_size++] = value;
  }

  constexpr void clear() {
    if (_data == nullptr) {
      return;
    }

    allocator::_free(_data);

    _data = nullptr;
    _size = 0;
    _capacity = 0;
  }

  constexpr T *data() { return _data; }
  constexpr const T *data() const { return _data; }

  constexpr const T &operator[](size_t pos) const {
    assert(pos < _size);
    return _data[pos];
  }
  constexpr T &operator[](size_t pos) {
    assert(pos < _size);
    return _data[pos];
  }

  Vector() = default;
  Vector(size_t size) { resize(size); };
  Vector(std::initializer_list<T> init_data) {
    reserve(init_data.size());

    for(const T& element : init_data) {
      push(element);
    }
  }
  Vector(const Vector &) = delete;
  Vector(Vector &&) = delete;
};

template <typename T, size_t SIZE>
struct Array {
  T _data[SIZE];

  constexpr static size_t size = SIZE;

  constexpr const T &operator[](size_t pos) const {
    assert(pos < SIZE);
    return _data[pos];
  }
  constexpr T &operator[](size_t pos) {
    assert(pos < SIZE);
    return _data[pos];
  }
};

template <typename T>
struct Span {
 private:
  const T *const _data;
  const size_t _data_size;

 public:
  Span(const T *const data, size_t data_size) : _data(data), _data_size(data_size) {}

  Span(const Vector<T> &vector) : _data(vector.data()), _data_size(vector.size()) {}

  constexpr size_t size() const { return _data_size; }

  constexpr const T *data() const { return _data; }

  constexpr const T &operator[](size_t pos) const {
    assert(pos < _data_size);
    return _data[pos];
  }
};

/*
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
