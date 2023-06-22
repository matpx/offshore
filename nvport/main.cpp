#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <nvrhi/validation.h>
#include <nvrhi/vulkan.h>

#include <iostream>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "VkBootstrap.h"

class CB : nvrhi::IMessageCallback {
  virtual void message(nvrhi::MessageSeverity severity, const char* messageText) {
    std::cout << messageText << std::endl;
  };
};

static CB cb;

int main() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_Window* sdl_window =
      SDL_CreateWindow("nvrhi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_VULKAN);

  vkb::InstanceBuilder builder;
  auto inst_ret = builder.set_app_name("Example Vulkan Application")
                      .request_validation_layers()
                      .require_api_version(VKB_VK_API_VERSION_1_2)
                      .use_default_debug_messenger()
                      .enable_validation_layers()
                      .build();
  if (!inst_ret) {
    std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
    return -1;
  }
  vkb::Instance vkb_inst = inst_ret.value();

  VkSurfaceKHR surface;
  if (SDL_Vulkan_CreateSurface(sdl_window, vkb_inst.instance, &surface) != SDL_TRUE) {
    std::cerr << "SDL_Vulkan_CreateSurface() failed\n";
    return -1;
  }

  VkPhysicalDeviceVulkan12Features features12 = {
    .timelineSemaphore = true,
  };

  vkb::PhysicalDeviceSelector selector{vkb_inst};
  auto phys_ret = selector.set_surface(surface)
                      .set_minimum_version(1, 2)
                      .set_required_features_12(features12)
                      .require_dedicated_transfer_queue()
                      .select();
  if (!phys_ret) {
    std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
    return -1;
  }

  vkb::DeviceBuilder device_builder{phys_ret.value()};
  // automatically propagate needed data from instance & physical device
  auto dev_ret = device_builder.build();
  if (!dev_ret) {
    std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
    return -1;
  }
  vkb::Device vkb_device = dev_ret.value();

  // Get the graphics queue with a helper function
  auto graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
  auto graphics_queue_index_ret = vkb_device.get_queue_index(vkb::QueueType::graphics);
  if (!graphics_queue_ret) {
    std::cerr << "Failed to get graphics queue. Error: " << graphics_queue_ret.error().message() << "\n";
    return -1;
  }
  VkQueue graphics_queue = graphics_queue_ret.value();

  vkb::SwapchainBuilder swapchain_builder{vkb_device};
  auto swap_ret = swapchain_builder.build();
  if (!swap_ret) {
    std::cerr << "Failed to create Swapchain. Error: " << swap_ret.error().message() << "\n";
    return -1;
  }
  vkb::Swapchain swapchain = swap_ret.value();

  const auto deviceExtensionsVector = vkb_device.physical_device.get_extensions();
  std::vector<const char*> deviceExtensions;

  for (const auto ext : deviceExtensionsVector) {
    deviceExtensions.push_back(ext.c_str());
  }

  nvrhi::vulkan::DeviceDesc deviceDesc = {};
  deviceDesc.errorCB = (nvrhi::IMessageCallback*)&cb;
  deviceDesc.instance = vkb_inst.instance;
  deviceDesc.physicalDevice = vkb_device.physical_device;
  deviceDesc.device = vkb_device.device;
  deviceDesc.graphicsQueue = graphics_queue;
  deviceDesc.graphicsQueueIndex = graphics_queue_index_ret.value();
  deviceDesc.deviceExtensions = deviceExtensions.data();
  deviceDesc.numDeviceExtensions = deviceExtensions.size();

  const vk::DynamicLoader dl;
  const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
      dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

  VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(deviceDesc.instance);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(deviceDesc.device);

  nvrhi::DeviceHandle nvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

  nvrhi::DeviceHandle nvrhiValidationLayer = nvrhi::validation::createValidationLayer(nvrhiDevice);
  nvrhiDevice = nvrhiValidationLayer;
}