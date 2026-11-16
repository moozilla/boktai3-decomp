#include "global.h"
#include "vorn.h"
#include "scene_actor_registry.h"

void Vorn_ParseField69A(struct Vorn *ctx)
{
    if (Script_SeekToKeyword(0x50) != 0) {
        ctx->frame69A = (Script_GetPc() != 0) ? Script_GetValue() : 300;
    } else {
        ctx->frame69A = 300;
    }
    ctx->field698 = ctx->frame69A;
}
