#include "material.hpp"

#include <sokol/sokol_gfx.h>

#include "../core/log.hpp"
#include "renderable.hpp"
#include "unlit.h"

namespace gfx::material {

static sg_shader unlit_shader = {};
static sg_pipeline basic_pipeline = {};

void init() {
  LOG_INFO("material::init()");

  unlit_shader = sg_make_shader(unlit_shader_desc(sg_query_backend()));

  sg_pipeline_desc basic_desc = {};
  basic_desc.shader = unlit_shader;
  basic_desc.layout.attrs[ATTR_vs_a_position] = {.format = SG_VERTEXFORMAT_FLOAT3};
  basic_desc.layout.attrs[ATTR_vs_a_normal] = {.format = SG_VERTEXFORMAT_FLOAT3};
  basic_desc.index_type = SG_INDEXTYPE_UINT16;
  basic_desc.depth = {
      .compare = SG_COMPAREFUNC_LESS_EQUAL,
      .write_enabled = true,
  };
  basic_desc.cull_mode = SG_CULLMODE_FRONT;

  basic_pipeline = sg_make_pipeline(basic_desc);
}

Material get() {
  return Material{basic_pipeline};
}

void finish() {
  sg_destroy_pipeline(basic_pipeline);
  sg_destroy_shader(unlit_shader);

  LOG_INFO("material::finish()");
}

}  // namespace gfx::material