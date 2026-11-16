#include "global.h"
#include "vorn.h"
#include "scene_actor_registry.h"

void Vorn_ParseField624(struct ChaseActor *actor)
{
    struct Vorn *ctx = (struct Vorn *)actor->base.context;

    if (Script_SeekToKeyword(0x78) != 0) {
        s32 val1, val2;

        if (Script_GetPc() != 0)
            val1 = Script_GetValue();
        else
            val1 = 0;
        ctx->field624 = val1;

        if (Script_GetPc() != 0)
            val2 = Script_GetValue();
        else
            val2 = 0;
        ctx->field626 = val2;
        ctx->field627 = 0;
    } else {
        ctx->field624 = 0;
        ctx->field626 = 0;
        ctx->field627 = 0;
    }
}
