#define gEffectPool gSceneStatusIndicatorManager
#include "global.h"
#include "ram.h"

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
void Effect_Remove(void *);

void EffectPool_ReleaseSlot(void *unused, struct PoolEffect *effect)
{
    struct EffectPool *pool;
    s32 slot;
    if (effect->state)
        effect->state = 0;
    pool = gSceneStatusIndicatorManager;
    slot = effect->slot;
    pool->activeMask &= ~(1 << slot);
    effect->active = 0;
    Effect_Remove(effect);
    effect->slot = -1;
    effect->age = 0;
}
