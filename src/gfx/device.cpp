#include "device.hpp"

#include <SDL2/SDL_vulkan.h>
#include <vk-bootstrap/VkBootstrap.h>

#include <tuple>

#include "../core/log.hpp"
#include "nvrhi/validation.h"
#include "nvrhi/vulkan.h"
#include "window.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace gfx::device {

static vkb::Instance vkb_inst;
static VkSurfaceKHR surface;
static vkb::Device vkb_device;
static vkb::Swapchain vkb_swapchain;

static vk::Device device = nullptr;
static vk::Queue present_queue = nullptr;
static vk::SwapchainKHR swapchain = nullptr;

static std::vector<std::tuple<vk::Image, nvrhi::TextureHandle>> swapchain_images;

static nvrhi::vulkan::DeviceHandle nvrhiDevice = nullptr;
static nvrhi::DeviceHandle nvrhiDeviceWrapped = nullptr;

static vk::Semaphore present_semaphore = nullptr;
static u32 swapchain_index = 0;

nvrhi::CommandListHandle barrier_command_list = nullptr;

void init() {
  u32 extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window::get_sdl_window(), &extension_count, nullptr);

  assert(extension_count > 0);

  static std::vector<const char*> instance_extensions(extension_count);
  SDL_Vulkan_GetInstanceExtensions(window::get_sdl_window(), &extension_count, instance_extensions.data());

  vkb::InstanceBuilder builder;
  builder.set_app_name("offshore")
      .require_api_version(VKB_VK_API_VERSION_1_2)
#ifndef NDEBUG
      .use_default_debug_messenger()
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

  device = vkb_device.device;

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

  auto present_queue_ret = vkb_device.get_queue(vkb::QueueType::present);
  if (!present_queue_ret) {
    const auto msg = present_queue_ret.error().message();
    FATAL("Failed to get present queue. Error: %s", msg.c_str());
  }

  present_queue = present_queue_ret.value();

  const nvrhi::Format swapchain_format = nvrhi::Format::SRGBA8_UNORM;
  const VkColorSpaceKHR swapchain_colorspace = (VkColorSpaceKHR)vk::ColorSpaceKHR::eSrgbNonlinear;

  vkb::SwapchainBuilder swapchain_builder{vkb_device};
  auto swap_ret = swapchain_builder
                      .set_desired_format({
                          .format = (VkFormat)nvrhi::vulkan::convertFormat(swapchain_format),
                          .colorSpace = swapchain_colorspace,
                      })
                      .set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                             VK_IMAGE_USAGE_SAMPLED_BIT)
                      .set_composite_alpha_flags(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
                      .build();

  if (!swap_ret) {
    const auto msg = swap_ret.error().message();
    FATAL("Failed to get Swapchain. Error: %s", msg.c_str());
  }
  vkb_swapchain = swap_ret.value();
  LOG_DEBUG("%d", vkb_swapchain.image_count);

  swapchain = vkb_swapchain.swapchain;

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
    deviceDesc.instanceExtensions = instance_extensions.data();
    deviceDesc.numInstanceExtensions = instance_extensions.size();
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
    nvrhiDeviceWrapped = nvrhi::validation::createValidationLayer(nvrhiDevice);
#endif
  }

  const auto images = vkb_swapchain.get_images().value();
  for (auto image : images) {
    std::tuple<vk::Image, nvrhi::TextureHandle> sci;
    std::get<0>(sci) = image;

    nvrhi::TextureDesc textureDesc;
    textureDesc.width = vkb_swapchain.extent.width;
    textureDesc.height = vkb_swapchain.extent.width;
    textureDesc.format = swapchain_format;
    textureDesc.debugName = "Swap chain image";
    textureDesc.initialState = nvrhi::ResourceStates::Present;
    textureDesc.keepInitialState = true;
    textureDesc.isRenderTarget = true;

    std::get<1>(sci) = nvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image,
                                                                 nvrhi::Object(std::get<0>(sci)), textureDesc);

    swapchain_images.push_back(sci);
  }

  barrier_command_list = nvrhiDevice->createCommandList();
  present_semaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
}

void begin_frame() {
  const vk::Result res = device.acquireNextImageKHR(swapchain, std::numeric_limits<uint64_t>::max(), present_semaphore,
                                                    vk::Fence(), &swapchain_index);

  assert(res == vk::Result::eSuccess);

  nvrhiDevice->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, present_semaphore, 0);
}

void finish_frame() {
  // auto framebufferDesc = nvrhi::FramebufferDesc().addColorAttachment(std::get<1>(swapchain_images[swapchain_index]));
  // nvrhi::FramebufferHandle framebuffer = nvrhiDevice->createFramebuffer(framebufferDesc);

  nvrhiDevice->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, present_semaphore, 0);

  barrier_command_list->open();  // umm...
  barrier_command_list->close();
  nvrhiDevice->executeCommandList(barrier_command_list);

  vk::PresentInfoKHR info = vk::PresentInfoKHR()
                                .setWaitSemaphoreCount(1)
                                .setPWaitSemaphores(&present_semaphore)
                                .setSwapchainCount(1)
                                .setPSwapchains(&swapchain)
                                .setPImageIndices(&swapchain_index);

  const vk::Result res = present_queue.presentKHR(&info);
  assert(res == vk::Result::eSuccess || res == vk::Result::eErrorOutOfDateKHR);

  present_queue.waitIdle();
}

void finish() {
  device.destroySemaphore(present_semaphore);
  barrier_command_list = nullptr;

  swapchain_images.clear();

  nvrhiDeviceWrapped = nullptr;
  nvrhiDevice = nullptr;

  vkb::destroy_swapchain(vkb_swapchain);
  vkb::destroy_device(vkb_device);
  vkb::destroy_surface(vkb_inst, surface);
  vkb::destroy_instance(vkb_inst);
}

}  // namespace gfx::device
