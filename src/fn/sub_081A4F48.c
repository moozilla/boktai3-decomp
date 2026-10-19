#include "global.h"

extern void *gUnk_02000244;
void *Hresvelgr_CreateProjectilePool(s32, s32, s32);
void Hresvelgr_InitializeProjectile(s32, s32, s32);

void Hresvelgr_SpawnProjectile(s32 a, s32 b, s32 c)
{
    if (gUnk_02000244 == 0)
        gUnk_02000244 = Hresvelgr_CreateProjectilePool(0, 0, 0);
    Hresvelgr_InitializeProjectile(a, b, c);
}
