#include "global.h"
#include "scene_actor_registry.h"

#ifdef NONMATCHING
// fuzzy: 92.3%
// foundry draft (worker best 92.3%); what differs is mostly register allocation and instruction order
#define Vorn_ParseField69A sub_080B30BC

struct VornData {
    u8 unknown0[0x698];
    u16 field698;
    u16 field69A;
};

void Vorn_ParseField69A(struct VornData *ctx)
{
    if (Script_SeekToKeyword(0x50) != 0) {
        s32 val;

        if (Script_GetPc() != 0)
            val = Script_GetValue();
        else
            val = 300;
        ctx->field69A = val;
    } else {
        ctx->field69A = 300;
    }
    ctx->field698 = ctx->field69A;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B30BC);
#endif
