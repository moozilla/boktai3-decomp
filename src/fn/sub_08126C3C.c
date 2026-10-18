#define EffectPool_UpdateActive sub_08126C3C
#define gEffectPoolUpdateCallbacks gUnk_08610764
#include "global.h"
struct PoolEffect {
    u8 unknown[0x3C];
    u8 kind;
    u8 unknown3D[2];
    u8 active;
};
struct EffectPool {
    u8 unknown[0x38];
    struct PoolEffect effects[128];
};
extern void *gUnk_08610764[];
s32 sub_08249240(void *, void *, void *);
s32 sub_08126C3C(struct EffectPool *pool)
{
    void **callbacks = gUnk_08610764;
    struct PoolEffect *effect = pool->effects;
    u8 *state = &effect->kind;
    s32 i = 127;
    do {
        if (state[3] == 1)
            sub_08249240(pool, effect, callbacks[state[0]]);
        effect++;
        state += 0x40;
    } while (--i >= 0);
    return 0;
}
