#include "nuklear_nvrhi.h"

#include <nvrhi/nvrhi.h>

#include <cassert>
#include <unordered_map>

#include "../core/log.hpp"
#include "../core/types.hpp"
#include "device.hpp"
#include "nuklear_main_ps.spirv.h"
#include "nuklear_main_vs.spirv.h"
#include "window.hpp"

struct nk_nvrhi_device {
  nk_buffer cmds;
  nk_buffer vertex_data;
  nk_buffer index_data;

  nvrhi::GraphicsPipelineHandle graphics_pipeline;
  nvrhi::BufferHandle vertex_buffer;
  nvrhi::BufferHandle index_buffer;
  nvrhi::TextureHandle font_texture;
  nvrhi::SamplerHandle font_sampler;
  nvrhi::BufferHandle constant_buffer;
  std::unordered_map<nvrhi::ITexture *, nvrhi::BindingSetHandle> bindings_cache;
};

struct nk_sdl_vertex {
  float position[2];
  float uv[2];
  nk_byte col[4];
};

static struct nk_sdl {
  SDL_Window *sdl_window;
  nk_nvrhi_device nvrhi_device;
  nk_context context;
  nk_font_atlas font_atlas;
} sdl;

NK_API void nk_sdl_device_create(void) {
  nk_nvrhi_device &dev = sdl.nvrhi_device;

  nk_buffer_init_default(&dev.cmds);
  nk_buffer_init_default(&dev.vertex_data);
  nk_buffer_init_default(&dev.index_data);

  nvrhi::ShaderDesc vertex_shader_desc(nvrhi::ShaderType::Vertex);
  vertex_shader_desc.entryName = "main_vs";

  nvrhi::ShaderHandle vertex_shader = gfx::device::get_device()->createShader(
      vertex_shader_desc, g_nuklear_main_vs_spirv, sizeof(g_nuklear_main_vs_spirv));

  const nvrhi::VertexAttributeDesc attributes[] = {
      nvrhi::VertexAttributeDesc()
          .setName("POSITION")
          .setFormat(nvrhi::Format::RG32_FLOAT)
          .setOffset(offsetof(nk_sdl_vertex, position))
          .setElementStride(sizeof(nk_sdl_vertex)),
      nvrhi::VertexAttributeDesc()
          .setName("UV")
          .setFormat(nvrhi::Format::RG32_FLOAT)
          .setOffset(offsetof(nk_sdl_vertex, uv))
          .setElementStride(sizeof(nk_sdl_vertex)),
      nvrhi::VertexAttributeDesc()
          .setName("COLOR")
          .setFormat(nvrhi::Format::RGBA8_UNORM)
          .setOffset(offsetof(nk_sdl_vertex, col))
          .setElementStride(sizeof(nk_sdl_vertex)),
  };

  const nvrhi::InputLayoutHandle input_layout =
      gfx::device::get_device()->createInputLayout(attributes, uint32_t(std::size(attributes)), vertex_shader);

  nvrhi::ShaderDesc pixel_shader_desc(nvrhi::ShaderType::Pixel);
  pixel_shader_desc.entryName = "main_ps";

  nvrhi::ShaderHandle pixel_shader = gfx::device::get_device()->createShader(pixel_shader_desc, g_nuklear_main_ps_spirv,
                                                                             sizeof(g_nuklear_main_ps_spirv));

  const auto layout_desc = nvrhi::BindingLayoutDesc()
                               .setVisibility(nvrhi::ShaderType::All)
                               .addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0))
                               .addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))
                               .addItem(nvrhi::BindingLayoutItem::Sampler(0));

  nvrhi::BindingLayoutHandle binding_layout = gfx::device::get_device()->createBindingLayout(layout_desc);

  nvrhi::BlendState blend_state;
  blend_state.targets[0]
      .setBlendEnable(true)
      .setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
      .setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha)
      .setSrcBlendAlpha(nvrhi::BlendFactor::SrcAlpha)
      .setDestBlendAlpha(nvrhi::BlendFactor::OneMinusSrcAlpha);

  const auto raster_state =
      nvrhi::RasterState().setFillSolid().setCullNone().setScissorEnable(true).setDepthClipEnable(true);

  const auto depth_stencil_state =
      nvrhi::DepthStencilState().disableDepthTest().enableDepthWrite().disableStencil().setDepthFunc(
          nvrhi::ComparisonFunc::Always);

  const auto pipeline_desc = nvrhi::GraphicsPipelineDesc()
                                 .addBindingLayout(binding_layout)
                                 .setInputLayout(input_layout)
                                 .setVertexShader(vertex_shader)
                                 .setPixelShader(pixel_shader)
                                 .setRenderState({
                                     .blendState = blend_state,
                                     .depthStencilState = depth_stencil_state,
                                     .rasterState = raster_state,
                                 });

  dev.graphics_pipeline =
      gfx::device::get_device()->createGraphicsPipeline(pipeline_desc, gfx::device::get_current_framebuffer());

  const auto constant_buffer_desc =
      nvrhi::BufferDesc().setByteSize(sizeof(mat4)).setIsConstantBuffer(true).setIsVolatile(true).setMaxVersions(256);

  dev.constant_buffer = gfx::device::get_device()->createBuffer(constant_buffer_desc);
}

NK_INTERN void nk_sdl_device_upload_atlas(const void *image, int width, int height) {
  struct nk_nvrhi_device &dev = sdl.nvrhi_device;

  const nvrhi::CommandListHandle command_list = gfx::device::get_device()->createCommandList();

  nvrhi::TextureDesc texture_desc;
  texture_desc.width = width;
  texture_desc.height = height;
  texture_desc.format = nvrhi::Format::RGBA8_UNORM;
  texture_desc.debugName = "Nuklear Font Texture";

  dev.font_texture = gfx::device::get_device()->createTexture(texture_desc);

  command_list->open();

  command_list->beginTrackingTextureState(dev.font_texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
  command_list->writeTexture(dev.font_texture, 0, 0, image, width * 4);
  command_list->setPermanentTextureState(dev.font_texture, nvrhi::ResourceStates::ShaderResource);
  command_list->commitBarriers();

  command_list->close();

  gfx::device::get_device()->executeCommandList(command_list);

  const auto sampler_desc =
      nvrhi::SamplerDesc().setAllAddressModes(nvrhi::SamplerAddressMode::ClampToEdge).setAllFilters(true);

  dev.font_sampler = gfx::device::get_device()->createSampler(sampler_desc);
}

NK_API void nk_sdl_device_destroy(void) {
  nk_nvrhi_device &dev = sdl.nvrhi_device;

  nk_buffer_free(&dev.cmds);
  nk_buffer_free(&dev.vertex_data);
  nk_buffer_free(&dev.index_data);

  dev = {};
}

void ensure_buffer_size(nvrhi::BufferHandle &buffer, const size_t required_size, const bool is_index_buffer) {
  assert(required_size > 0);

  if (buffer == nullptr || buffer->getDesc().byteSize < required_size) {  // TODO extra buffer
    nvrhi::BufferDesc vertex_buffer_desc =
        nvrhi::BufferDesc()
            .setByteSize(required_size)
            .setDebugName(is_index_buffer ? "Nuklear Index Buffer" : "Nuklear Vertex Buffer")
            .setIsVertexBuffer(!is_index_buffer)
            .setIsIndexBuffer(is_index_buffer)
            .setInitialState(is_index_buffer ? nvrhi::ResourceStates::IndexBuffer : nvrhi::ResourceStates::VertexBuffer)
            .setKeepInitialState(true);

    buffer = gfx::device::get_device()->createBuffer(vertex_buffer_desc);

    const std::string &debug_name = buffer->getDesc().debugName;
    LOG_DEBUG("resize nuklear buffer \"%s\" to %ld byte", debug_name.c_str(), required_size);
  }
}

nvrhi::IBindingSet *get_binding_set(nvrhi::ITexture *texture) {
  struct nk_nvrhi_device &dev = sdl.nvrhi_device;

  const auto iter = dev.bindings_cache.find(texture);

  if (iter != dev.bindings_cache.end()) {
    return iter->second;
  }

  const auto binding_set_desc = nvrhi::BindingSetDesc()
                                    .addItem(nvrhi::BindingSetItem::ConstantBuffer(0, dev.constant_buffer))
                                    .addItem(nvrhi::BindingSetItem::Texture_SRV(0, texture))
                                    .addItem(nvrhi::BindingSetItem::Sampler(0, dev.font_sampler));

  const std::string &debug_name = texture->getDesc().debugName;
  LOG_DEBUG("create nuklear binding set \"%s\"", debug_name.c_str());

  nvrhi::BindingSetHandle binding_set =
      gfx::device::get_device()->createBindingSet(binding_set_desc, dev.graphics_pipeline->getDesc().bindingLayouts[0]);

  dev.bindings_cache[texture] = binding_set;

  return binding_set;
}

NK_API void nk_sdl_render() {
  nk_nvrhi_device &dev = sdl.nvrhi_device;

  const vec2 width_height = gfx::window::get_width_height();
  const mat4 ortho = glm::ortho(0.0f, width_height.x, width_height.y, 0.0f);

  const nvrhi::CommandListHandle command_list = gfx::device::get_device()->createCommandList();
  command_list->open();

  static const nk_draw_vertex_layout_element vertex_layout[] = {
      {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_sdl_vertex, position)},
      {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_sdl_vertex, uv)},
      {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(nk_sdl_vertex, col)},
      {NK_VERTEX_LAYOUT_END}};

  nk_convert_config config = {};
  config.vertex_layout = vertex_layout;
  config.vertex_size = sizeof(nk_sdl_vertex);
  config.vertex_alignment = NK_ALIGNOF(nk_sdl_vertex);
  config.circle_segment_count = 22;
  config.curve_segment_count = 22;
  config.arc_segment_count = 22;
  config.global_alpha = 1.0f;
  config.shape_AA = NK_ANTI_ALIASING_ON;
  config.line_AA = NK_ANTI_ALIASING_ON;

  nk_convert(&sdl.context, &dev.cmds, &dev.vertex_data, &dev.index_data, &config);

  ensure_buffer_size(dev.vertex_buffer, nk_buffer_total(&dev.vertex_data), false);
  ensure_buffer_size(dev.index_buffer, nk_buffer_total(&dev.index_data), true);

  command_list->writeBuffer(dev.vertex_buffer, nk_buffer_memory_const(&dev.vertex_data),
                            nk_buffer_total(&dev.vertex_data));
  command_list->writeBuffer(dev.index_buffer, nk_buffer_memory_const(&dev.index_data),
                            nk_buffer_total(&dev.index_data));

  command_list->writeBuffer(dev.constant_buffer, glm::value_ptr(ortho), sizeof(ortho));

  const nk_draw_command *cmd = nullptr;
  nk_draw_index offset = 0;
  nk_draw_foreach(cmd, &sdl.context, &dev.cmds) {
    if (!cmd->elem_count) continue;

    const ivec2 window_size = gfx::window::get_width_height();

    nvrhi::ViewportState viewport =
        nvrhi::ViewportState()
            .addViewport(nvrhi::Viewport(window_size.x, window_size.y))
            .addScissorRect(nvrhi::Rect(std::max(0.0f, cmd->clip_rect.x), cmd->clip_rect.x + cmd->clip_rect.w,
                                        std::max(0.0f, cmd->clip_rect.y), cmd->clip_rect.y + cmd->clip_rect.h));

    nvrhi::ITexture *texture =
        cmd->texture.ptr == nullptr ? dev.font_texture.Get() : (nvrhi::ITexture *)cmd->texture.ptr;

    nvrhi::GraphicsState graphics_state =
        nvrhi::GraphicsState()
            .setFramebuffer(gfx::device::get_current_framebuffer())
            .setPipeline(dev.graphics_pipeline)
            .addVertexBuffer({.buffer = dev.vertex_buffer})
            .setIndexBuffer({.buffer = dev.index_buffer, .format = nvrhi::Format::R16_UINT})
            .addBindingSet(get_binding_set(texture))
            .setViewport(viewport);

    nvrhi::DrawArguments draw_arguments;
    draw_arguments.vertexCount = cmd->elem_count;
    draw_arguments.startIndexLocation = offset;

    command_list->setGraphicsState(graphics_state);
    command_list->drawIndexed(draw_arguments);

    offset += cmd->elem_count;
  }

  nk_buffer_clear(&dev.cmds);
  nk_buffer_clear(&dev.vertex_data);
  nk_buffer_clear(&dev.index_data);

  command_list->close();
  gfx::device::get_device()->executeCommandList(command_list);
}

static void nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit *edit) {
  const char *text = SDL_GetClipboardText();
  if (text) nk_textedit_paste(edit, text, nk_strlen(text));
  (void)usr;
}

static void nk_sdl_clipboard_copy(nk_handle usr, const char *text, int len) {
  char *str = 0;
  (void)usr;
  if (!len) return;
  str = (char *)malloc((size_t)len + 1);
  if (!str) return;
  memcpy(str, text, (size_t)len);
  str[len] = '\0';
  SDL_SetClipboardText(str);
  free(str);
}

NK_API void nk_sdl_init_font_stash() {
  nk_font_atlas_init_default(&sdl.font_atlas);
  nk_font_atlas_begin(&sdl.font_atlas);

  const void *image;
  int w, h;
  image = nk_font_atlas_bake(&sdl.font_atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
  nk_sdl_device_upload_atlas(image, w, h);
  nk_font_atlas_end(&sdl.font_atlas, nk_handle_ptr(sdl.nvrhi_device.font_texture), 0);
  if (sdl.font_atlas.default_font) nk_style_set_font(&sdl.context, &sdl.font_atlas.default_font->handle);
}

NK_API struct nk_context *nk_sdl_init(SDL_Window *win) {
  sdl.sdl_window = win;
  nk_init_default(&sdl.context, 0);
  sdl.context.clip.copy = nk_sdl_clipboard_copy;
  sdl.context.clip.paste = nk_sdl_clipboard_paste;
  sdl.context.clip.userdata = nk_handle_ptr(0);
  nk_sdl_device_create();

  nk_sdl_init_font_stash();

  return &sdl.context;
}

NK_API int nk_sdl_handle_event(SDL_Event *evt) {
  struct nk_context *ctx = &sdl.context;

  // if (ctx->input.mouse.grab) {
  //   SDL_SetRelativeMouseMode(SDL_TRUE);
  //   ctx->input.mouse.grab = 0;
  // } else if (ctx->input.mouse.ungrab) {
  //   int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
  //   SDL_SetRelativeMouseMode(SDL_FALSE);
  //   SDL_WarpMouseInWindow(sdl.sdl_window, x, y);
  //   ctx->input.mouse.ungrab = 0;
  // }

  switch (evt->type) {
    case SDL_KEYUP:
    case SDL_KEYDOWN: {
      int down = evt->type == SDL_KEYDOWN;
      const Uint8 *state = SDL_GetKeyboardState(0);
      switch (evt->key.keysym.sym) {
        case SDLK_RSHIFT:
        case SDLK_LSHIFT:
          nk_input_key(ctx, NK_KEY_SHIFT, down);
          break;
        case SDLK_DELETE:
          nk_input_key(ctx, NK_KEY_DEL, down);
          break;
        case SDLK_RETURN:
          nk_input_key(ctx, NK_KEY_ENTER, down);
          break;
        case SDLK_TAB:
          nk_input_key(ctx, NK_KEY_TAB, down);
          break;
        case SDLK_BACKSPACE:
          nk_input_key(ctx, NK_KEY_BACKSPACE, down);
          break;
        case SDLK_HOME:
          nk_input_key(ctx, NK_KEY_TEXT_START, down);
          nk_input_key(ctx, NK_KEY_SCROLL_START, down);
          break;
        case SDLK_END:
          nk_input_key(ctx, NK_KEY_TEXT_END, down);
          nk_input_key(ctx, NK_KEY_SCROLL_END, down);
          break;
        case SDLK_PAGEDOWN:
          nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
          break;
        case SDLK_PAGEUP:
          nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
          break;
        case SDLK_z:
          nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_r:
          nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_c:
          nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_v:
          nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_x:
          nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_b:
          nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_e:
          nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
          break;
        case SDLK_UP:
          nk_input_key(ctx, NK_KEY_UP, down);
          break;
        case SDLK_DOWN:
          nk_input_key(ctx, NK_KEY_DOWN, down);
          break;
        case SDLK_LEFT:
          if (state[SDL_SCANCODE_LCTRL])
            nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
          else
            nk_input_key(ctx, NK_KEY_LEFT, down);
          break;
        case SDLK_RIGHT:
          if (state[SDL_SCANCODE_LCTRL])
            nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
          else
            nk_input_key(ctx, NK_KEY_RIGHT, down);
          break;
      }
    }
      return 1;

    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN: {
      int down = evt->type == SDL_MOUSEBUTTONDOWN;
      const int x = evt->button.x, y = evt->button.y;
      switch (evt->button.button) {
        case SDL_BUTTON_LEFT:
          if (evt->button.clicks > 1) nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
          nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
          break;
        case SDL_BUTTON_MIDDLE:
          nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
          break;
        case SDL_BUTTON_RIGHT:
          nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
          break;
      }
    }
      return 1;

    case SDL_MOUSEMOTION:
      if (ctx->input.mouse.grabbed) {
        int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
        nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
      } else
        nk_input_motion(ctx, evt->motion.x, evt->motion.y);
      return 1;

    case SDL_TEXTINPUT: {
      nk_glyph glyph;
      memcpy(glyph, evt->text.text, NK_UTF_SIZE);
      nk_input_glyph(ctx, glyph);
    }
      return 1;

    case SDL_MOUSEWHEEL:
      nk_input_scroll(ctx, nk_vec2((float)evt->wheel.x, (float)evt->wheel.y));
      return 1;
  }
  return 0;
}

NK_API
void nk_sdl_shutdown(void) {
  nk_font_atlas_clear(&sdl.font_atlas);
  nk_free(&sdl.context);
  nk_sdl_device_destroy();
  sdl = {};
}
