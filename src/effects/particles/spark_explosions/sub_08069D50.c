#include "effects/particles/burst_emitter.h"
s32 SparkExplosion_Create(const struct ContactPosition *position, u32 mode, u32 context, s32 a, s32 b,
                        s32 c, s32 d)
{
    if (position) {
        struct BurstEmitterPool *pool = gBurstEmitterPool;
        if (!pool) {
            pool = SparkExplosion_CreatePool();
            if (!pool)
                return 0;
        }
        {
            struct BurstEmitter *emitter = SparkExplosion_AllocateSlot(pool);
            if (emitter) {
                emitter->state = 1;
                emitter->position = *position;
                emitter->frame = 0;
                emitter->mode = mode;
                emitter->context = context;
                SparkExplosion_InitCollider(emitter, a, b, c, d);
            }
        }
    }
    return 0;
}
