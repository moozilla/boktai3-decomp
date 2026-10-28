#include "area_effect_link.h"
// MATCHING_CONCESSION: register-pinning
s32 Div(s32, s32);
s32 AreaEffect_ScaleLinkResourceGain(s32 gain)
{
    u32 value;
    register u32 product asm("r2");
    if (!gLinkSession)
        value = 0;
    else
        value = gLinkSession->value;
    product = value;
    product *= gain;
    return Div(product, 140);
}
