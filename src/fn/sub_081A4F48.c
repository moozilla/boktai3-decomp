#include "global.h"
#include "ram.h"

void *Hresvelgr_CreateProjectilePool(s32, s32, s32);
void Hresvelgr_InitializeProjectile(s32, s32, s32);

void Hresvelgr_SpawnProjectile(s32 a, s32 b, s32 c)
{
    if (gHresvelgrProjectilePool == 0)
        gHresvelgrProjectilePool = Hresvelgr_CreateProjectilePool(0, 0, 0);
    Hresvelgr_InitializeProjectile(a, b, c);
}
