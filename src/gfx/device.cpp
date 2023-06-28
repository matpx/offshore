#include "device.hpp"

#include <SDL2/SDL_vulkan.h>
#include <vk-bootstrap/VkBootstrap.h>

#include "../core/log.hpp"
#include "nvrhi/validation.h"
#include "nvrhi/vulkan.h"
#include "window.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace gfx::device {

class NvrhiMessageCallback : nvrhi::IMessageCallback {
  void message([[maybe_unused]] nvrhi::MessageSeverity severity, const char* messageText) override {
    LOG_ERROR("nvrhi: %s", messageText);
  };
};

static const NvrhiMessageCallback message_callback;

static vkb::Instance vkb_instance;
static VkSurfaceKHR vk_surface;
static vkb::Device vkb_device;
static vkb::Swapchain vkb_swapchain;

static vk::Device vk_device = nullptr;
static vk::Queue vk_present_queue = nullptr;
static vk::SwapchainKHR vk_swapchain = nullptr;

static nvrhi::vulkan::DeviceHandle nvrhi_device_vk;
static nvrhi::DeviceHandle nvrhi_device;

static std::vector<std::tuple<vk::Image, nvrhi::TextureHandle>> swapchain_image_handles;
static std::vector<nvrhi::FramebufferHandle> nvrhi_framebuffers;

static vk::Semaphore vk_present_semaphore = nullptr;
static std::vector<nvrhi::EventQueryHandle> query_pool;

static u32 current_swapchain_index = 0;
static u32 current_frame_index = 0;

nvrhi::CommandListHandle barrier_command_list;
nvrhi::CommandListHandle clear_command_list;

nvrhi::DeviceHandle get_device() { return nvrhi_device; }

nvrhi::FramebufferHandle get_current_framebuffer() { return nvrhi_framebuffers[current_swapchain_index]; }

uvec2 get_current_viewport() {
  const auto framebuffer_info = get_current_framebuffer()->getFramebufferInfo();

  return {framebuffer_info.width, framebuffer_info.height};
}

void init() {
  // Instance
  std::vector<const char*> instance_extensions;
  {
    u32 extension_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window::get_sdl_window(), &extension_count, nullptr);

    instance_extensions.resize(extension_count);
    SDL_Vulkan_GetInstanceExtensions(window::get_sdl_window(), &extension_count, instance_extensions.data());

    vkb::InstanceBuilder instance_builder;
    instance_builder.set_app_name("offshore")
        .require_api_version(VKB_VK_API_VERSION_1_3)
#ifndef NDEBUG
        .enable_validation_layers()
#endif
        .use_default_debug_messenger();

    for (const char* extension : instance_extensions) {
      instance_builder.enable_extension(extension);
    }

    vkb::Result<vkb::Instance> instance_builder_ret = instance_builder.build();
    if (!instance_builder_ret) {
      const std::string msg = instance_builder_ret.error().message();
      FATAL("Failed to create Vulkan instance. Error: %s", msg.c_str());
    }
    vkb_instance = instance_builder_ret.value();
  }

  // Surface
  if (SDL_Vulkan_CreateSurface(gfx::window::get_sdl_window(), vkb_instance.instance, &vk_surface) != SDL_TRUE) {
    FATAL("SDL_Vulkan_CreateSurface() failed");
  }

  // Device
  {
    // const VkPhysicalDeviceFeatures features = {
    //     .imageCubeArray = true,
    //     .geometryShader = true,
    //     .tessellationShader = true,
    //     .dualSrcBlend = true,
    //     .samplerAnisotropy = true,
    //     .textureCompressionBC = true,
    //     .shaderImageGatherExtended = true,
    // };

    const VkPhysicalDeviceVulkan12Features features12 = {
        // .descriptorIndexing = true,
        // .shaderSampledImageArrayNonUniformIndexing = true,
        // .descriptorBindingPartiallyBound = true,
        // .descriptorBindingVariableDescriptorCount = true,
        // .runtimeDescriptorArray = true,
        .timelineSemaphore = true,
        // .bufferDeviceAddress = true,
    };

    // const VkPhysicalDeviceVulkan13Features features13 = {
    //     .synchronization2 = true,
    //     .maintenance4 = vk::PhysicalDeviceMaintenance4Features().maintenance4,
    // };

    // const std::vector<const char*> desired_extensions = {
    //     "VK_EXT_DEBUG_MARKER_EXTENSION_NAME",          "VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME",
    //     "VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME", "VK_NV_MESH_SHADER_EXTENSION_NAME",
    //     "VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME", "VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME",
    //     "VK_KHR_MAINTENANCE_4_EXTENSION_NAME",
    // };

    vkb::PhysicalDeviceSelector device_selector{vkb_instance};
    vkb::Result<vkb::PhysicalDevice> device_selector_ret = device_selector.set_surface(vk_surface)
                                                               .set_minimum_version(1, 3)
                                                               // .set_required_features(features)
                                                               .set_required_features_12(features12)
                                                               // .set_required_features_13(features13)
                                                               // .add_desired_extensions(desired_extensions)
                                                               .select();

    if (!device_selector_ret) {
      const std::string msg = device_selector_ret.error().message();
      FATAL("Failed to select Vulkan Physical Device. Error: %s", msg.c_str());
    }

    vkb::DeviceBuilder device_builder{device_selector_ret.value()};
    vkb::Result<vkb::Device> device_ret = device_builder.build();
    if (!device_ret) {
      const std::string msg = device_ret.error().message();
      FATAL("Failed to create Vulkan device. Error: %s", msg.c_str());
    }

    vkb_device = device_ret.value();
    vk_device = vkb_device.device;
  }

  // Queue
  VkQueue graphics_queue = nullptr;
  u32 graphics_queue_index = 0;
  {
    vkb::Result<VkQueue> graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
      const std::string msg = graphics_queue_ret.error().message();
      FATAL("Failed to get graphics queue. Error: %s", msg.c_str());
    }

    vkb::Result<u32> graphics_queue_index_ret = vkb_device.get_queue_index(vkb::QueueType::graphics);
    if (!graphics_queue_index_ret) {
      const std::string msg = graphics_queue_index_ret.error().message();
      FATAL("Failed to get graphics queue index. Error: %s", msg.c_str());
    }

    vkb::Result<VkQueue> present_queue_ret = vkb_device.get_queue(vkb::QueueType::present);
    if (!present_queue_ret) {
      const std::string msg = present_queue_ret.error().message();
      FATAL("Failed to get present queue. Error: %s", msg.c_str());
    }

    graphics_queue = graphics_queue_ret.value();
    graphics_queue_index = graphics_queue_index_ret.value();
    vk_present_queue = present_queue_ret.value();
  }

  // Swapchain
  const nvrhi::Format swapchain_format = nvrhi::Format::BGRA8_UNORM;
  const VkColorSpaceKHR swapchain_colorspace = (VkColorSpaceKHR)vk::ColorSpaceKHR::eSrgbNonlinear;
  {
    vkb::SwapchainBuilder swapchain_builder{vkb_device};

    const vkb::Result<vkb::Swapchain> swapchain_builder_ret =
        swapchain_builder
            .set_desired_format({
                .format = (VkFormat)nvrhi::vulkan::convertFormat(swapchain_format),
                .colorSpace = swapchain_colorspace,
            })
            .set_desired_extent(window::get_width_height().x, window::get_width_height().y)
            .set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                   VK_IMAGE_USAGE_SAMPLED_BIT)
            .set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
            .set_composite_alpha_flags_alternative(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
            // .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .build();

    if (!swapchain_builder_ret) {
      const std::string msg = swapchain_builder_ret.error().message();
      FATAL("Failed to get Swapchain. Error: %s", msg.c_str());
    }

    vkb_swapchain = swapchain_builder_ret.value();
    vk_swapchain = vkb_swapchain.swapchain;
  }

  // NVRHI
  {
    const std::vector<std::string> device_extensions = vkb_device.physical_device.get_extensions();
    std::vector<const char*> device_extensions_cstr;

    for (const std::string& extension : device_extensions) {
      device_extensions_cstr.push_back((extension.c_str()));
    }

    nvrhi::vulkan::DeviceDesc deviceDesc = {};
    deviceDesc.errorCB = (nvrhi::IMessageCallback*)&message_callback;
    deviceDesc.instance = vkb_instance.instance;
    deviceDesc.physicalDevice = vkb_device.physical_device;
    deviceDesc.device = vkb_device.device;
    deviceDesc.graphicsQueue = graphics_queue;
    deviceDesc.graphicsQueueIndex = graphics_queue_index;
    deviceDesc.instanceExtensions = instance_extensions.data();
    deviceDesc.numInstanceExtensions = instance_extensions.size();
    deviceDesc.deviceExtensions = device_extensions_cstr.data();
    deviceDesc.numDeviceExtensions = device_extensions_cstr.size();

    const vk::DynamicLoader dl;
    const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(deviceDesc.instance);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(deviceDesc.device);

    nvrhi_device_vk = nvrhi::vulkan::createDevice(deviceDesc);

#ifndef NDEBUG
    nvrhi_device = nvrhi::validation::createValidationLayer(nvrhi_device_vk);
#else
    nvrhi_device = nvrhi_device_vk;
#endif
  }

  // Textures
  {
    const std::vector<VkImage> swapchain_images = vkb_swapchain.get_images().value();
    for (const VkImage& image : swapchain_images) {
      std::tuple<vk::Image, nvrhi::TextureHandle> image_handle;
      std::get<0>(image_handle) = image;

      nvrhi::TextureDesc textureDesc = {
          .width = vkb_swapchain.extent.width,
          .height = vkb_swapchain.extent.height,
          .format = swapchain_format,
          .debugName = "Swap chain image",
          .isRenderTarget = true,
          .initialState = nvrhi::ResourceStates::Present,
          .keepInitialState = true,
      };

      std::get<1>(image_handle) = nvrhi_device->createHandleForNativeTexture(
          nvrhi::ObjectTypes::VK_Image, nvrhi::Object(std::get<0>(image_handle)), textureDesc);

      const auto depth_texture_desc = nvrhi::TextureDesc{
          .width = vkb_swapchain.extent.width,
          .height = vkb_swapchain.extent.height,
          .format = nvrhi::Format::D24S8,
          .dimension = nvrhi::TextureDimension::Texture2D,
          .debugName = "Depth Buffer",
          .isRenderTarget = true,
          .useClearValue = true,
          .initialState = nvrhi::ResourceStates::DepthWrite,
          .keepInitialState = true,
      };

      const nvrhi::TextureHandle depth_texture = nvrhi_device->createTexture(depth_texture_desc);

      swapchain_image_handles.push_back(image_handle);

      const nvrhi::FramebufferDesc framebuffer_desc =
          nvrhi::FramebufferDesc()
              .addColorAttachment(std::get<1>(image_handle), nvrhi::AllSubresources)
              .setDepthAttachment(depth_texture);

      nvrhi_framebuffers.push_back(nvrhi_device->createFramebuffer(framebuffer_desc));
    }
  }

  // Helper
  barrier_command_list = nvrhi_device->createCommandList();
  clear_command_list = nvrhi_device->createCommandList();

  vk_present_semaphore = vk_device.createSemaphore(vk::SemaphoreCreateInfo());

  for (u32 i = 0; i < max_frames_in_flight; i++) {
    query_pool.push_back(nvrhi_device->createEventQuery());
  }
}

void begin_frame() {
  [[maybe_unused]] const vk::Result res = vk_device.acquireNextImageKHR(
      vk_swapchain, std::numeric_limits<uint64_t>::max(), vk_present_semaphore, vk::Fence(), &current_swapchain_index);

  assert(res == vk::Result::eSuccess);

  nvrhi_device_vk->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, vk_present_semaphore, 0);

  clear_command_list->open();

  const auto framebuffer_desc = get_current_framebuffer()->getDesc();
  clear_command_list->clearTextureFloat(framebuffer_desc.colorAttachments[0].texture,
                                        framebuffer_desc.colorAttachments[0].subresources,
                                        nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f));
  clear_command_list->clearDepthStencilTexture(framebuffer_desc.depthAttachment.texture,
                                               framebuffer_desc.depthAttachment.subresources, true, 1.0f, true, 0.0f);

  clear_command_list->close();
  nvrhi_device->executeCommandList(clear_command_list);
}

void finish_frame() {
  nvrhi_device_vk->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, vk_present_semaphore, 0);

  barrier_command_list->open();  // umm...
  barrier_command_list->close();
  nvrhi_device->executeCommandList(barrier_command_list);

  vk::PresentInfoKHR info = vk::PresentInfoKHR()
                                .setWaitSemaphoreCount(1)
                                .setPWaitSemaphores(&vk_present_semaphore)
                                .setSwapchainCount(1)
                                .setPSwapchains(&vk_swapchain)
                                .setPImageIndices(&current_swapchain_index);

  [[maybe_unused]] const vk::Result res = vk_present_queue.presentKHR(&info);
  assert(res == vk::Result::eSuccess || res == vk::Result::eErrorOutOfDateKHR);

#ifndef NDEBUG
  vk_present_queue.waitIdle();
#endif

  nvrhi_device->waitEventQuery(query_pool[current_frame_index]);
  nvrhi_device->resetEventQuery(query_pool[current_frame_index]);
  nvrhi_device->setEventQuery(query_pool[current_frame_index], nvrhi::CommandQueue::Graphics);

  current_frame_index = (current_frame_index + 1) % max_frames_in_flight;

  nvrhi_device->runGarbageCollection();
}

void wait_idle() { nvrhi_device->waitForIdle(); }

void finish() {
  vk_device.destroySemaphore(vk_present_semaphore);
  clear_command_list = nullptr;
  barrier_command_list = nullptr;

  nvrhi_framebuffers.clear();
  swapchain_image_handles.clear();

  nvrhi_device = nullptr;
  nvrhi_device_vk = nullptr;

  vkb::destroy_swapchain(vkb_swapchain);
  vkb::destroy_device(vkb_device);
  vkb::destroy_surface(vkb_instance, vk_surface);
  vkb::destroy_instance(vkb_instance);
}

}  // namespace gfx::device
