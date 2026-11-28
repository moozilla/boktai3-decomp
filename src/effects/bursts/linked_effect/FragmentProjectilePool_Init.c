#include "effects/bursts/linked_effect.h"

s32 FragmentProjectilePool_Init(struct LinkedEffectPool *pool)
{
    pool->sequences = ScriptProc_Start(0x922E, 0x1DC3);
    pool->occupied = 0;
    gActorEffectManager = (u8 *)pool;
    return 0;
}
