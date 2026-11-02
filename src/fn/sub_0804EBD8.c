#include "area_effect_link.h"
s32 Div(s32, s32);
static inline u16 linkResourceScale(struct AreaEffectLinkState *link)
{
    if (!link)
        return 0;
    return link->value;
}
s32 AreaEffect_ScaleLinkResourceGain(s32 gain)
{
    u32 value = linkResourceScale(gLinkSession);
    return Div((u32)value * gain, 140);
}
