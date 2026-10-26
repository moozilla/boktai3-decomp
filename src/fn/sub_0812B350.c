#include "global.h"
#include "ram.h"

struct ActorEffect {
    u8 unk_00[0x176];
    u8 state;
    u8 unk_177[3];
    u8 entered;
    u8 unk_17B[0x11];
};

struct ActorEffectManager {
    u8 unk_00[0x1C];
    u32 used;
    struct ActorEffect slots[8];
};

static inline u8 IsOccupied(u32 *mask, s32 index, u32 one)
{
    if (*mask & (one << index))
        return TRUE;
    return FALSE;
}

void FragmentProjectilePool_RequestRemovalAll(void)
{
    struct ActorEffectManager *manager = gActorEffectManager;
    struct ActorEffect *effect;
    s32 i;
    u32 one;
    u32 stride;

    if (manager) {
        i = 0;
        one = 1;
        effect = manager->slots;
        stride = sizeof(*effect);
        do {
            if (IsOccupied(&manager->used, i, one)) {
                if (effect->state != 3) {
                    effect->state = 3;
                    effect->entered = one;
                }
            }
            effect = (void *)((u8 *)effect + stride);
            i++;
        } while (i < 8);
    }
}
