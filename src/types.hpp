#pragma once

#include <glm/ext/quaternion_float.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cstdint>

// primitive

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// HMM

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

using mat4 = glm::mat4;

using quat = glm::quat;

// GFX

using index_t = u16;
