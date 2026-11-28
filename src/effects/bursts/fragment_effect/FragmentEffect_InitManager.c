#include "effects/bursts/fragment_effect.h"

s32 FragmentEffect_InitManager(struct FragmentEffectPool *pool)
{
    pool->occupied = 0;
    AnimData_Load(pool->graphics, 0xF422);
    gFragmentEffectManager = (u8 *)pool;
    return 0;
}
