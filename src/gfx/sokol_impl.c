#define NK_IMPLEMENTATION
#include <nuklear/nuklear.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/util/sokol_nuklear.h>
#include <sokol/util/sokol_shape.h>

struct nk_context* get_nk_context(void) { return &_snuklear.ctx; }
