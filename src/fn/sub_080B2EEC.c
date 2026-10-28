// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"

#define gRandomTableIndex gRandomIndex

s32 SceneActorRegistry_CountLiving(void);
s32 SceneActorRegistry_CountLivingByCategory(s32);
void SceneActorReaction_UpdateFacingAttack(void);
static inline u8 Inactive(struct SceneActorRegistryResetView *p)
{
    if (p) {
        u32 mask = 0x40;
        u32 offset = 0x2D4;
        if (p->flags2D4 & mask)
            goto inactive;
        {
            u32 mask = 0x20000;
            offset -= 4;
            if (p->flags2D0 & mask)
                goto inactive;
        }
        if (p->health > 0)
            goto active;
    inactive:
        return 1;
    }
active:
    return 0;
}
static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 RandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(const u8 *)(gRandomTable + gRandomIndex);
}
static inline void Clear(u32 *p, u32 mask)
{
    *p &= ~mask;
}
inline static s32 RandomByte_TrySoloAction(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return (u8)((const u16 *)0x0203B400)[gRandomIndex];
}

void SceneActor_TrySoloAction(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryResetView *q = p;
    u8 zero;
    s32 t1;
    u32 timer;
    if (Inactive(p))
        return;
    {
        u32 mask = 0xE33F;
        if (p->flags2D4 & mask)
            return;
    }
    if (!q->field688)
        return;
    t1 = 4;
    if (!Test(&q->flags6D4, t1))
        return;
    if (SceneActorRegistry_CountLivingByCategory(0x19) > 1)
        return;
    if (SceneActorRegistry_CountLiving() > 1)
        return;
    timer = q->timer68D;
    zero = timer;
    asm("" : "+r"(zero));
    if (zero)
        q->timer68D = timer - 1;
    else {
        q->timer68D = 0xF0;
        if (RandomByte_TrySoloAction() <= 0x9F)
            return;
        if (!q->field688)
            return;
        {
            u32 state = 3;
            p->state2A8 = state;
        }
        p->phase2A9 = zero;
        p->behavior2AA = zero;
        p->counter2AC = zero;
        p->field2B1 = 1;
        {
            u32 one = 1;
            void *callback = SceneActorReaction_UpdateFacingAttack;
            u32 mode = 0x2A;
            p->field2B2 = one;
            p->field2B0 = zero;
            p->field2B5 = mode;
            p->target = callback;
        }
        p->field2C8 = zero;
        Clear(&p->flags114, 1);
        p->word104 = zero;
    }
}
