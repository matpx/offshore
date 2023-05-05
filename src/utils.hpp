#pragma once

#ifndef NDEBUG
#include <stb/stb_leakcheck.h>
#endif

#include "types.hpp"
#include "log.hpp"
#include <stb/stb_ds.h>

namespace utils {

    template<typename T>
    struct Vector {
        T* _data = nullptr;

        constexpr size_t size() const        { return arrlenu(_data); }
        constexpr void set_size(size_t size) { arrsetlen(_data, size); }

        constexpr T pop()             { return arrpop(_data); }

        constexpr void push(T& value) { arrpush(_data, value); }
        constexpr void push(const T& value) { arrpush(_data, value); }

        constexpr void clear() { arrfree(_data); }

        constexpr const T& operator[](size_t pos) const { assert(pos < size()); return _data[pos]; }
        constexpr T& operator[](size_t pos)             { assert(pos < size()); return _data[pos]; }
    };

}
