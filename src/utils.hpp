#pragma once

#ifndef NDEBUG
#include <stb/stb_leakcheck.h>
#endif

#include "types.hpp"
#include "log.hpp"

#define STBDS_NO_SHORT_NAMES
#include <stb/stb_ds.h>

namespace utils {

    template<typename T>
    struct Vector {
        T* _data = nullptr;

        constexpr size_t size() const        { return stbds_arrlenu(_data); }
        constexpr void set_size(size_t size) { stbds_arrsetlen(_data, size); }

        constexpr T pop() { return stbds_arrpop(_data); }

        constexpr void push(T& value)       { stbds_arrpush(_data, value); }
        constexpr void push(const T& value) { stbds_arrpush(_data, value); }

        constexpr void clear() { stbds_arrfree(_data); }

        constexpr const T& operator[](size_t pos) const { assert(pos < size()); return _data[pos]; }
        constexpr       T& operator[](size_t pos)       { assert(pos < size()); return _data[pos]; }
    };

    template<typename KEY, typename VALUE>
    struct Hashmap {

        struct Internal {
            KEY key;
            VALUE value;
        };

        Internal *_data = nullptr;

        constexpr bool has(KEY key) { return stbds_hmgetp_null(_data, key) != nullptr; }

        constexpr const VALUE& get_or_default(KEY key) { return stbds_hmget(_data, key); }
        constexpr       VALUE* get_or_null(KEY key) {
            Internal *i = stbds_hmgetp_null(_data, key);
            return i != nullptr ? &(i->value) : nullptr;
        }

        constexpr void put(KEY key, VALUE value) { stbds_hmput(_data, key, value); }
        constexpr bool remove(KEY key)           { return stbds_hmdel(_data, key); }
        constexpr void clear()                   { stbds_hmfree(_data); }

        constexpr VALUE& operator[](KEY key) {
            Internal *i = stbds_hmgetp_null(_data, key);
            assert(i != nullptr);
            return i->value;
        }
    };

}
