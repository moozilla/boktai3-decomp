#include "global.h"
#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fuzzy: 80.6%
// foundry draft (worker best 80.6%); what differs is mostly register allocation and instruction order
#define SceneActorReaction_FinishAnimation17 sub_080ED838
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Ivy_UpdateActiveState(void);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}
void sub_080ED838(struct SceneActorRegistryResetView *actor)
{
    u32 offset;
    if (TakePrepared(actor)) {
        u16 *flags = &actor->colliderA.flags;
        u32 value = *flags;
        u32 clear = 0xFFFB;
        clear &= value;
        clear |= 8;
        *flags = clear;
        {
            u16 *fields = &actor->flags2C0;
            u32 mask = ~0x800;
            *fields = mask & *fields;
        }
        SceneActorAnimation_SelectDirectionalEntry(actor, 17);
    }
    offset = 0x2B3;
    if (*((u8 *)actor + offset)) {
        void *callback = Ivy_UpdateActiveState;
        u32 kind = 34;
        u8 *field = &actor->field2B2;
        u32 zero = 0;
        u32 one = 1;
        u32 targetOffset;
        *field = one;
        offset -= 3;
        *((u8 *)actor + offset) = zero;
        offset += 5;
        *((u8 *)actor + offset) = kind;
        targetOffset = 0x21C;
        *(void **)((u8 *)actor + targetOffset) = callback;
        field = (u8 *)&actor->field2C8;
        offset = 0;
        *(u16 *)field = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
        {
            u32 stateOffset;
            u32 behavior = 2;
            stateOffset = 0x2A8;
            *((u8 *)actor + stateOffset) = offset;
            stateOffset++;
            *((u8 *)actor + stateOffset) = offset;
            actor->behavior2AA = behavior;
            actor->counter2AC = zero;
            stateOffset += 8;
            *((u8 *)actor + stateOffset) = one;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080ED838);
#endif
