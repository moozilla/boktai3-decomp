#include "effects/feathers/feather_effect_pool.h"
#include "engine/engine.h"

s32 FeatherEffectPool_Init(struct FeatherPool *pool)
{
    AnimData_Load(pool->graphics, 0x84B4);
    pool->sequences = (void *)ScriptProc_Start(0x922E, 0x4EC0);
    pool->used = 0;
    gFadingSpritePool = pool;
    return 0;
}
