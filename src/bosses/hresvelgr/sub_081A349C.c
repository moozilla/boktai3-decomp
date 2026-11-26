#include "global.h"

void Hresvelgr_ReleaseShot(u8 *);

void Hresvelgr_OnShotCollision(u32 a, u32 b, u8 *p)
{
    if (*(u16 *)(p + 0x8A)) {
        Hresvelgr_ReleaseShot(p);
    }
}
