#include "device.hpp"

#include <SDL2/SDL_vulkan.h>
#include <vk-bootstrap/VkBootstrap.h>

#include "../core/log.hpp"
#include "nvrhi/validation.h"
#include "nvrhi/vulkan.h"
#include "window.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

static vkb::Instance vkb_inst;
static VkSurfaceKHR surface;
static vkb::Device vkb_device;
static vkb::Swapchain swapchain;

static nvrhi::DeviceHandle nvrhiDevice = nullptr;

namespace gfx::device {

void init() {
  u32 extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window::get_sdl_window(), &extension_count, nullptr);

  assert(extension_count > 0);

  std::vector<const char*> instance_extensions(extension_count);
  SDL_Vulkan_GetInstanceExtensions(window::get_sdl_window(), &extension_count, instance_extensions.data());

  vkb::InstanceBuilder builder;
  builder.set_app_name("Example Vulkan Application")
      .use_default_debug_messenger()
      .require_api_version(VKB_VK_API_VERSION_1_2)
#ifndef NDEBUG
      .enable_validation_layers();
#endif

  for (const auto extension : instance_extensions) {
    builder.enable_extension(extension);
  }

  auto inst_ret = builder.build();
  if (!inst_ret) {
    const auto msg = inst_ret.error().message();
    FATAL("Failed to create Vulkan instance. Error: %s", msg.c_str());
  }
  vkb_inst = inst_ret.value();

  ;
  if (SDL_Vulkan_CreateSurface(gfx::window::get_sdl_window(), vkb_inst.instance, &surface) != SDL_TRUE) {
    FATAL("SDL_Vulkan_CreateSurface() failed");
  }

  VkPhysicalDeviceVulkan12Features features12 = {
      .timelineSemaphore = true,
  };

  vkb::PhysicalDeviceSelector selector{vkb_inst};
  auto phys_ret = selector.set_surface(surface).set_minimum_version(1, 2).set_required_features_12(features12).select();
  if (!phys_ret) {
    const auto msg = phys_ret.error().message();
    FATAL("Failed to select Vulkan Physical Device. Error: %s", msg.c_str());
  }

  vkb::DeviceBuilder device_builder{phys_ret.value()};
  auto dev_ret = device_builder.build();
  if (!dev_ret) {
    const auto msg = dev_ret.error().message();
    FATAL("Failed to create Vulkan device. Error: %s", msg.c_str());
  }
  vkb_device = dev_ret.value();

  auto graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
  if (!graphics_queue_ret) {
    const auto msg = graphics_queue_ret.error().message();
    FATAL("Failed to get graphics queue. Error: %s", msg.c_str());
  }

  auto graphics_queue_index_ret = vkb_device.get_queue_index(vkb::QueueType::graphics);
  if (!graphics_queue_index_ret) {
    const auto msg = graphics_queue_index_ret.error().message();
    FATAL("Failed to get graphics queue index. Error: %s", msg.c_str());
  }

  vkb::SwapchainBuilder swapchain_builder{vkb_device};
  auto swap_ret = swapchain_builder.set_composite_alpha_flags(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR).build();

  if (!swap_ret) {
    const auto msg = swap_ret.error().message();
    FATAL("Failed to get Swapchain. Error: %s", msg.c_str());
  }
  swapchain = swap_ret.value();

  const std::vector<std::string> device_extensions = vkb_device.physical_device.get_extensions();
  static std::vector<const char*> device_extensions_cstr;

  std::transform(device_extensions.begin(), device_extensions.end(), std::back_inserter(device_extensions_cstr),
                 [](const auto& s) { return s.c_str(); });

  class NvrhiMessageCallback : nvrhi::IMessageCallback {
    void message([[maybe_unused]] nvrhi::MessageSeverity severity, const char* messageText) override {
      LOG_INFO("nvrhi: %s", messageText);
    };
  };

  const static NvrhiMessageCallback message_callback;

  {
    nvrhi::vulkan::DeviceDesc deviceDesc = {};
    deviceDesc.errorCB = (nvrhi::IMessageCallback*)&message_callback;
    deviceDesc.instance = vkb_inst.instance;
    deviceDesc.physicalDevice = vkb_device.physical_device;
    deviceDesc.device = vkb_device.device;
    deviceDesc.graphicsQueue = graphics_queue_ret.value();
    deviceDesc.graphicsQueueIndex = graphics_queue_index_ret.value();
    deviceDesc.deviceExtensions = device_extensions_cstr.data();
    deviceDesc.numDeviceExtensions = device_extensions_cstr.size();

    const static vk::DynamicLoader dl;
    const static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(deviceDesc.instance);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(deviceDesc.device);

    nvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

#ifndef NDEBUG
    nvrhiDevice = nvrhi::validation::createValidationLayer(nvrhiDevice);
#endif
  }
}

void finish() {
  nvrhiDevice = nullptr;

  vkb::destroy_swapchain(swapchain);
  vkb::destroy_device(vkb_device);
  vkb::destroy_surface(vkb_inst, surface);
  vkb::destroy_instance(vkb_inst);
}

}  // namespace gfx::device
