#include "area_effect_link.h"
#define AreaEffect_ScaleLinkResourceGain sub_0804EBD8
// MATCHING_CONCESSION: register-pinning
s32 Div(s32, s32);
s32 AreaEffect_ScaleLinkResourceGain(s32 gain)
{
    u32 value;
    register u32 product asm("r2");
    if (!gUnk_020004B4)
        value = 0;
    else
        value = gUnk_020004B4->value;
    product = value;
    product *= gain;
    return Div(product, 140);
}
