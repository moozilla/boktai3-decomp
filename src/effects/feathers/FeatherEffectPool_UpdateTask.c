#include "effects/feathers/feather_effect_pool.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
extern u32 gUnk_02000560;
static inline u8 test(u32 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline u32 Paused(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 Locked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
u32 FeatherEffectPool_UpdateTask(struct FeatherPool *pool)
{
    struct Feather *first = pool->slots;
    if (pool->used && !Paused() && !Locked()) {
        s32 i = 0;
        FeatherStateCallback const *table = gUnk_086107A0;
        for (; i < 20; i++) {
            if (test(&pool->used, 1 << i))
                table[first[i].state](pool, &first[i]);
        }
    }
    return 0;
}
