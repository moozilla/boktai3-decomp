#include "global.h"
#include "engine.h"
#include "ram.h"
/* Tentative family name; visual identity is not yet established. */
#define gActorEffectStateCallbacks gUnk_086107C8
struct ActorEffect {
    u8 unknown0[0x176];
    u8 state;
    u8 unknown177[0x18c - 0x177];
};
struct ActorEffectManager {
    u8 task[24];
    void *sequences;
    u32 used;
    struct ActorEffect slots[8];
};
extern u32 gUnk_02000560;
extern void *gUnk_086107C8[];
s32 sub_08249240(void *, void *, void *);
static inline u32 paused(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 locked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
static inline u8 occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return 1;
    return 0;
}
s32 ActorEffect_UpdatePool(struct ActorEffectManager *manager)
{
    s32 i;
    void **table;
    u32 state;
    struct ActorEffect *effect;
    if (!paused()) {
        if (!locked()) {
            i = 0;
            table = gUnk_086107C8;
            state = (u32)&manager->slots[0].state;
            effect = manager->slots;
            do {
                if (occupied(&manager->used, i))
                    sub_08249240(manager, effect, table[*(u8 *)state]);
                {
                    u32 stride = sizeof(*effect);
                    state += stride;
                    effect++;
                }
                i++;
            } while (i < 8);
        }
    }
    return 0;
}
