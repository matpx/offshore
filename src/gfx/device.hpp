#pragma once

#include <nvrhi/nvrhi.h>

namespace gfx::device {

void init();

void begin_frame();

void finish_frame();

void finish();

nvrhi::DeviceHandle get_device();

}  // namespace gfx::device
