#define EffectPool_ReleaseSlot sub_081267D4
#define gEffectPool gUnk_02000158
#include "global.h"

struct PoolEffect {
    u8 unknown[0x28];
    u16 age;
    u8 active;
    s8 slot;
    u16 state;
};
struct EffectPool {
    u8 unknown[0x25C];
    u32 activeMask;
};
extern struct EffectPool *gUnk_02000158;
void Effect_Remove(void *);

void sub_081267D4(void *unused, struct PoolEffect *effect)
{
    struct EffectPool *pool;
    s32 slot;
    if (effect->state)
        effect->state = 0;
    pool = gUnk_02000158;
    slot = effect->slot;
    pool->activeMask &= ~(1 << slot);
    effect->active = 0;
    Effect_Remove(effect);
    effect->slot = -1;
    effect->age = 0;
}
