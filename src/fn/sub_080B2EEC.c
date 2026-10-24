#include "global.h"
#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// foundry draft (worker best 89.2%); what differs is mostly register allocation and instruction order
#define gRandomTableIndex gUnk_03005308
#define gRandomTable      gUnk_0203B400
extern u32 gUnk_03005308;

s32 SceneActorRegistry_CountLiving(void);
s32 SceneActorRegistry_CountLivingByCategory(s32);
void SceneActorReaction_UpdateFacingAttack(void);
static inline u8 Inactive(struct SceneActorRegistryResetView *p)
{
    if (p) {
        u32 mask = 0x40;
        u32 offset = 0x2D4;
        if (*(u16 *)((u8 *)p + offset) & mask)
            return 1;
        {
            u32 mask = 0x20000;
            offset -= 4;
            if (*(u32 *)((u8 *)p + offset) & mask)
                return 1;
        }
        if (p->health <= 0)
            return 1;
    }
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
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    return *(const u8 *)(gUnk_0203B400 + gUnk_03005308);
}
static inline void Clear(u32 *p, u32 mask)
{
    *p &= ~mask;
}
inline static s32 RandomByte_TrySoloAction(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    return *((const u8 *)((*((typeof(gUnk_0203B400) *)0x0203B400)) + gUnk_03005308));
}

void SceneActor_TrySoloAction(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryResetView *q = p;
    u8 zero;
    s32 t1;
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
    zero = q->timer68D;
    if (zero)
        q->timer68D = zero - 1;
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

#else
INCLUDE_ASM("asm/nonmatching", SceneActor_TrySoloAction);
#endif
