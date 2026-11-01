#include "global.h"
#include "scene_actor_reaction.h"
/* When flags2D0 bit 27 and flags2D4 bit 13 are set and field2B8 is live, switch to control kind 29 (SceneActorReaction_UpdateTimedBurst) and reset hit state. */
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorReaction_UpdateTimedBurst(void);
static inline u8 Test(u32 value)
{
    if (value)
        return 1;
    return 0;
}
static inline u32 Flag2C0(struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2C0 & mask;
}
static inline u32 Mask(u16 *flags, u32 maskInput)
{
    u32 mask = maskInput;
    return *flags & mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 zero)
{
    void *callback = SceneActorReaction_UpdateTimedBurst;
    u32 kind = 29;
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = callback;
    {
        u32 off = 0x2C8;
        {
            u16 *field = (void *)((u8 *)actor + off);
            *field = zero;
        }
    }
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
u32 SceneActorReaction_TryBeginControl29(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 0x8000000;
    u32 *flags2D0 = &actor->flags2D0;
    if (*flags2D0 & mask) {
        u32 blocked = 0x8006;
        u16 *flags2D4;
        u32 flags;
        {
            u16 *address = &actor->flags2D4;
            flags = *address;
            blocked &= flags;
            flags2D4 = address;
        }
        if (!blocked && (flags & 0x2000) && actor->field2B8) {
            struct ContactCollider *collider;
            if (actor->field2B5 == 29)
                return 1;
            SetControl(actor, blocked);
            {
                u32 state = 3;
                actor->state2A8 = state;
                actor->phase2A9 = 0;
                actor->behavior2AA = 0;
                actor->counter2AC = blocked;
                actor->field2B1 = 1;
            }
            SceneActorTerrain_ClearCollisionTimers((void *)actor);
            {
                u32 clear = ~0x13;
                *flags2D4 = clear & *flags2D4;
            }
            {
                u32 clear = ~2;
                *flags2D0 &= clear;
            }
            {
                u16 *flags2D6 = &actor->flags2D6;
                u32 clear = ~0x9CB;
                *flags2D6 = clear & *flags2D6;
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
            SceneActorEffects_AdvanceSteps(actor, 4);
            actor->group.field5C4 = blocked;
            actor->group.field5C0 = blocked;
            actor->group.field5C2 = blocked;
            collider = &actor->colliderA;
            if (Test(Flag2C0(actor, 0x40)))
                collider->flags &= ~0x100;
            else if ((actor->field2DC & 0x84000) || (Mask(flags2D4, 0x2016)))
                collider->flags &= ~0x100;
            else
                collider->flags |= 0x100;
            return 1;
        }
    }
    return 0;
}
