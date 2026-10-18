#include "global.h"
#include "scene_actor_registry.h"


struct VornData {
    u8 unknown0[0x688];
    s32 field688;
    u8 field68C;
};

void Vorn_ParseField688(struct VornData *ctx)
{
    if (Script_SeekToKeyword(0x73) != 0) {
        s32 val1;

        if (Script_GetPc() != 0)
            val1 = Script_GetValue();
        else
            val1 = 0;
        ctx->field68C = val1;

        if ((u8)val1 == 1) {
            ctx->field688 = 0;
        } else {
            s32 *p = &ctx->field688;
            s32 val2;

            if (Script_GetPc() != 0)
                val2 = Script_GetValue();
            else
                val2 = 0;
            *p = val2;
        }
    }
}
