#include "global.h"
#include "area_effect_link.h"

struct LinkScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
void Script_Call(u32, struct LinkScriptArgs *);
static inline u8 *LinkSession_GetData(struct AreaEffectLinkState *p)
{
    u8 *flag;
    if (!p)
        return NULL;
    flag = &p->unavailable;
    if (*flag)
        return NULL;
    return flag + 0x8D;
}
void LinkSession_CallPositionScript(struct AreaEffectPosition *position, s32 value)
{
    s32 values[6];
    struct LinkScriptArgs args;
    struct AreaEffectLinkState *p = gLinkSession;
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
