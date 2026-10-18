#include "global.h"

void Nidhoggr_ClearEffectPools(void *);
void Nidhoggr_DestroyParts(void *);
void Nidhoggr_DestroySuspendedObjectSprite(void *);
void Nidhoggr_UnregisterTerrainCells(void *);

s32 Nidhoggr_Destroy(u8 *t)
{
    Nidhoggr_ClearEffectPools(t + 0x54);
    Nidhoggr_DestroyParts(t);
    Nidhoggr_DestroySuspendedObjectSprite(t);
    Nidhoggr_UnregisterTerrainCells(t);
    return 0;
}
