
#ifdef NONMATCHING
// fuzzy: 96.1%
// foundry draft (worker best 96.1%); what differs is mostly register allocation and instruction order
#define LinkSession_CallPositionScript sub_081093FC
#include "global.h"
#include "area_effect_link.h"
struct LinkScriptArgs {
    u32 count:16, unused:16;
    s32 *values;
};
void Script_Call(u32, struct LinkScriptArgs *);
static inline u8 *LinkSession_GetData(struct AreaEffectLinkState *p)
{
    if (!p || p->unavailable) return NULL;
    return p->sessionData;
}
void sub_081093FC(struct AreaEffectPosition *position, s32 value)
{
    s32 values[6];
    struct LinkScriptArgs args;
    struct AreaEffectLinkState *p = gUnk_020004B4;
    if (LinkSession_GetData(p)) {
        values[0] = position->x;
        values[1] = position->y;
        values[2] = position->z;
        values[3] = 0;
        values[4] = value;
        values[5] = 0;
        args.count = 6;
        args.values = values;
        Script_Call(p->positionScript, &args);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081093FC);
#endif
