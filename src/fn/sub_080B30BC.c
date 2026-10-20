#include "global.h"
#include "scene_actor_registry.h"

struct VornData {
    u8 unknown0[0x698];
    u16 field698;
    u16 field69A;
};

void Vorn_ParseField69A(struct VornData *ctx)
{
    if (Script_SeekToKeyword(0x50) != 0) {
        ctx->field69A = (Script_GetPc() != 0) ? Script_GetValue() : 300;
    } else {
        ctx->field69A = 300;
    }
    ctx->field698 = ctx->field69A;
}
