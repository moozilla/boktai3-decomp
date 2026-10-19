#include "global.h"
/* Vorn_ParseField624_0808B320: same code as Vorn_ParseField624 at another address (ported by relatives.py; provisional name) */
#define Vorn_ParseField624_0808B320 sub_0808B320
#include "scene_actor_registry.h"

struct VornData {
    u8 unknown0[0x624];
    u16 field624;
    u8 field626;
    u8 field627;
};

void Vorn_ParseField624_0808B320(struct SceneActorRegistryActor *actor)
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
