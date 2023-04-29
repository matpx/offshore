#pragma once

#include <hmm/HandmadeMath.h>
#include <cstdint>

// primitive

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

// HMM

using vec2 = HMM_Vec2;
using vec3 = HMM_Vec3;
using vec4 = HMM_Vec4;

using mat2 = HMM_Mat2;
using mat3 = HMM_Mat3;
using mat4 = HMM_Mat4;

using quat = HMM_Quat;

constexpr vec2 v2(float x, float y) {
    return HMM_V2(x, y);
}

constexpr vec3 v3(float x, float y, float z) {
    return HMM_V3(x, y, z);
}

constexpr vec4 v4(float x, float y, float z, float w) {
    return HMM_V4(x, y, z, w);
}

constexpr quat q(float x, float y, float z, float w) {
    return HMM_Q(x, y, z, w);
}

inline mat4 m4() {
    return HMM_M4();
}

// GFX

using index_type = u16;

