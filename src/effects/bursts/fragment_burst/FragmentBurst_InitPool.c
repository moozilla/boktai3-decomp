#include "effects/bursts/fragment_burst.h"

s32 FragmentBurst_InitPool(struct FragmentBurstPool *pool)
{
    AnimData_Load(pool->graphics, 0xD45E);
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x1DC3);
    pool->occupied = 0;
    gSpriteGroupPool = pool;
    return 0;
}
