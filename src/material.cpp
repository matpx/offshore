#include "material.hpp"
#include <sokol/sokol_gfx.h>

#include "log.hpp"
#include "renderable.hpp"
#include "unlit.h"

namespace gfx::material {

sg_shader unlit_shader = {};
sg_pipeline basic_pipeline = {};

void init() {
  LOG_INFO("material::init()");

  unlit_shader = sg_make_shader(unlit_shader_desc(sg_query_backend()));

  sg_pipeline_desc basic_desc = {};
  basic_desc.shader = unlit_shader;
  basic_desc.layout.attrs[ATTR_vs_position] = {.format = SG_VERTEXFORMAT_FLOAT3};
  basic_desc.index_type = SG_INDEXTYPE_UINT16;

  basic_pipeline = sg_make_pipeline(basic_desc);
}

Material get() {
    return Material {
        basic_pipeline
    };
}

void finish() {
  sg_destroy_pipeline(basic_pipeline);
  sg_destroy_shader(unlit_shader);

  LOG_INFO("material::finish()");
}

}