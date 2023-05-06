#pragma once

#include <hmm/HandmadeMath.h>

#include <cassert>

#include "types.hpp"

namespace world {

using EntityId = i32;

constexpr EntityId INVALID_ENTITY = -1;

struct Transform {
    vec3 translation = HMM_V3(0, 0, 0);
    quat rotation = HMM_Q(0, 0, 0, 1);

    mat4 world = HMM_M4D(1);

    void update() {
        world = HMM_Translate(translation) * HMM_QToM4(rotation);
    }
};

struct Camera {
    u32 width;
    u32 height;
    float fov;
    float near;
    float far;

    mat4 projection;

    Camera(u32 width, u32 height, float fov, float near, float far)
        : width(width), height(height), fov(fov), near(near), far(far) {
        update();
    }

    void update() {
        projection = HMM_Perspective_RH_ZO(fov, (float)width / (float)height, near, far);
    }
};

struct Entity {
    enum class Variant : u8 {
        INVALID = 0,
        Camera,
    };

    Transform transform;

    union {
        Camera camera;
    };

    Variant variant;

    Entity(Transform transform, Camera component)
        : transform(transform), camera(component), variant(Variant::Camera) {}

    constexpr Camera& get_camera() {
        assert(variant == Variant::Camera);

        return camera;
    }
};

EntityId create(const Entity&);

Entity& get(EntityId);

void finish();

extern EntityId main_camera;

}  // namespace world
