#include "burst_emitter.h"
#define BurstEmitter_Create sub_08069D50
s32 BurstEmitter_Create(const struct ContactPosition *position, u32 mode, u32 context, s32 a, s32 b,
                        s32 c, s32 d)
{
    if (position) {
        struct BurstEmitterPool *pool = gUnk_02000160;
        if (!pool) {
            pool = sub_08069F4C();
            if (!pool)
                return 0;
        }
        {
            struct BurstEmitter *emitter = sub_08069D0C(pool);
            if (emitter) {
                emitter->state = 1;
                emitter->position = *position;
                emitter->frame = 0;
                emitter->mode = mode;
                emitter->context = context;
                sub_08069BC8(emitter, a, b, c, d);
            }
        }
    }
    return 0;
}
