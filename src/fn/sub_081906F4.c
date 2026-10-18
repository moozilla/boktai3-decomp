#include "global.h"

void Nidhoggr_ClearEffectPools(u8 *);

u32 ScenePanelEffect_DestroyTask(u8 *p)
{
    Nidhoggr_ClearEffectPools(p + 0x18);
    return 0;
}
