#include "global.h"

void EffectEntry_Release(u8 *);

void Hresvelgr_OnHomingShotCollision(u32 a, u32 b, u8 *p)
{
    if (*(u16 *)(p + 0x15E)) {
        EffectEntry_Release(p);
    }
}
