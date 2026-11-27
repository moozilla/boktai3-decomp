#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_reaction.h"
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
static inline u32 Flag2C0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.placementFlags & mask;
}
static inline u32 Mask(u16 *flags, u32 maskInput)
{
    u32 mask = maskInput;
    return *flags & mask;
}
u32 SceneActorReaction_TryBeginControl29(struct ChaseActor *actor)
{
    u32 mask = 0x8000000;
    u32 *flags2D0 = &actor->base.statusFlags;
    if (*flags2D0 & mask) {
        u32 blocked = 0x8006;
        u16 *flags2D4;
        u32 flags;
        {
            u16 *address = &actor->base.motionFlags;
            flags = *address;
            blocked &= flags;
            flags2D4 = address;
        }
        if (!blocked && (flags & 0x2000) && actor->base.hitLockTimer) {
            struct ContactCollider *collider;
            if (actor->base.controlKind == 29)
                return 1;
            SceneActorTerrain_InstallControl(actor, SceneActorReaction_UpdateTimedBurst, 29);
            SceneActorTerrain_EnterState(actor, 3, 0, 0);
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
                u16 *flags2D6 = &actor->base.reactionFlags;
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
            collider = &actor->base.colliderA;
            if (Test(Flag2C0(actor, 0x40)))
                collider->flags &= ~0x100;
            else if ((actor->base.hitReactionFlags & 0x84000) || (Mask(flags2D4, 0x2016)))
                collider->flags &= ~0x100;
            else
                collider->flags |= 0x100;
            return 1;
        }
    }
    return 0;
}
