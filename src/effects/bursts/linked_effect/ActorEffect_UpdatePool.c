#include "effects/bursts/linked_effect.h"

extern u32 gUnk_02000560;
extern void *gUnk_086107C8[];
s32 sub_08249240(void *, void *, void *);

static inline u32 Locked(void)
{
    u32 mask = 4;

    return gUnk_02000560 & mask;
}

static inline u32 Paused(void)
{
    u32 mask = 0x400;

    return (gEngineFlags | gScenePauseFlags) & mask;
}

s32 ActorEffect_UpdatePool(struct LinkedEffectPool *pool)
{
    s32 i;

    if (!(Paused())) {
        if (!Locked()) {
            for (i = 0; i < 8; i++) {
                if (LinkedEffectPool_IsOccupied(pool, i))
                    sub_08249240(pool, &pool->effects[i], gUnk_086107C8[pool->effects[i].state]);
            }
        }
    }
    return 0;
}
