#pragma once

#include <nvrhi/nvrhi.h>

#include <span>

#include "../core/types.hpp"

namespace gfx::device {

constexpr u32 max_frames_in_flight = 2;

void init();

void begin_frame();

void finish_frame();

void wait_idle();

void finish();

nvrhi::DeviceHandle get_device();

nvrhi::FramebufferHandle get_current_framebuffer();

uvec2 get_current_viewport();

}  // namespace gfx::device
