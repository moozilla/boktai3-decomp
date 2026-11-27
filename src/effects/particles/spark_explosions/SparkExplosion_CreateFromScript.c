#include "effects/particles/burst_emitter.h"
static inline s32 SparkExplosion_GetScriptValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline s32 SparkExplosion_GetScriptShort(void)
{
    if (Script_GetPc())
        return (s16)Script_GetValue();
    return 0;
}
void SparkExplosion_CreateFromScript(void)
{
    struct BurstEmitterPool *pool = gBurstEmitterPool;
    struct BurstEmitter *emitter;
    s32 a, b, c, d;
    if (!pool) {
        pool = SparkExplosion_CreatePool();
        if (!pool)
            return;
    }
    emitter = SparkExplosion_AllocateSlot(pool);
    if (!emitter)
        return;
    if (Script_SeekToKeyword('p')) {
        emitter->position.x = SparkExplosion_GetScriptValue();
        emitter->position.y = SparkExplosion_GetScriptValue();
        emitter->position.z = SparkExplosion_GetScriptValue();
    }
    if (Script_SeekToKeyword('P')) {
        a = SparkExplosion_GetScriptShort();
        b = SparkExplosion_GetScriptShort();
        c = SparkExplosion_GetScriptShort();
        d = SparkExplosion_GetScriptShort();
    } else {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
    }
    emitter->context = Script_GetKeywordValue('c', 0);
    emitter->state = 1;
    emitter->frame = 0;
    emitter->mode = 2;
    SparkExplosion_InitCollider(emitter, a, b, c, d);
}
