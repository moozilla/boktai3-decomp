#include "effects/feathers/feather_effect_pool.h"
#include "engine/engine_pause.h"

u32 FeatherEffectPool_UpdateTask(struct FeatherPool *pool)
{
    struct Feather *slots = pool->slots;

    if (pool->used && !Engine_PauseBits(0x400) && !Feather_LockBits(4)) {
        s32 i;

        for (i = 0; i < 20; i++) {
            if (FeatherPool_IsUsed(pool, i))
                gUnk_086107A0[slots[i].state](pool, &slots[i]);
        }
    }
    return 0;
}
