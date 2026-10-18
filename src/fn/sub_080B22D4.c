#include "global.h"
#include "scene_actor_registry.h"

#define Vorn_ParseField624 sub_080B22D4

struct VornData {
    u8 unknown0[0x624];
    u16 field624;
    u8 field626;
    u8 field627;
};

void Vorn_ParseField624(struct SceneActorRegistryActor *actor)
{
    struct VornData *ctx = (struct VornData *)actor->context;

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
