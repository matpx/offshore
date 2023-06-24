#pragma once

#include <span>
#include <nvrhi/nvrhi.h>

namespace gfx::device {

void init();

void begin_frame();

void finish_frame();

void finish();

nvrhi::DeviceHandle get_device();

nvrhi::FramebufferHandle get_current_framebuffer();

}  // namespace gfx::device
