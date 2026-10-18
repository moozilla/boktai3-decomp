#include "global.h"

u32 SceneProjectile_CreatePool(u8 *);
u32 SunOrb_GetOrCreateManager(u32);

s32 SceneBurstEmitter_Init(u8 *p)
{
    u32 r = SceneProjectile_CreatePool(p);
    *(u32 *)(p + 0x18) = r;
    if (r != 0) {
        r = SunOrb_GetOrCreateManager(r);
        *(u32 *)(p + 0x1C) = r;
        if (r != 0) {
            return 0;
        }
    }
    return -1;
}
