#include "global.h"

extern u32 gUnk_02000078;
u32 FloatingEffectManager_Create(void);

u32 FloatingEffectManager_GetOrCreate(void)
{
    u32 r = gUnk_02000078;
    if (r == 0) {
        return FloatingEffectManager_Create();
    }
    return r;
}
