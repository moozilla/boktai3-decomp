#include "global.h"
#include "engine.h"
#include "area_effect.h"
s32 AreaEffect_Create(u32, u32);
s32 AreaEffect_CreateLink(u32, u32);
s32 AreaEffect_CreateOnce(u32 a, u32 b)
{
    s32 r = gAreaEffectManager;
    if (r == 0) {
        if ((gEngineFlags & 0x800) == 0)
            r = AreaEffect_Create(a, b);
        else
            r = AreaEffect_CreateLink(a, b);
    }
    return r;
}
