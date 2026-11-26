#include "engine/shared/scene_actor_chase_shared.h"
#include "engine/shared/global.h"
#include "engine/shared/scene_actor_chase.h"
#include "engine/shared/scene_actor_terrain_reaction.h"
#include "engine/shared/scene_actor_status.h"

// confidence=low: preserves the existing tentative reaction-state interpretation.

// confidence=low: target contact flags enable a delayed return to route behavior.
static inline u8 HasTargetFlag(struct SceneActorReactionMotion **out,
                               struct SceneActorReactionMotion *motion, u32 mask)
{
    u32 value = motion->flags & mask;
    *out = motion;
    if (value)
        return 1;
    return 0;
}
static inline void SaveDelay(u8 *packet, void *control, u8 behavior)
{
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = behavior;
}
static inline void ClearTargetFlags(struct SceneActorReactionMotion *motion, u32 mask)
{
    motion->flags &= mask;
}
static inline struct SceneActorReactionMotion *MakeMotionPointer(void *actor)
{
    u8 *bytes = actor;
    return (void *)(bytes + 0xdc);
}
u8 SceneActorReaction_BeginTargetRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorReactionMotion *motion;
    if (!ActorStatus_BehaviorFlags(actor, 0x400000) ||
        !ActorStatus_BehaviorFlags(actor, 0x200000) ||
        !HasTargetFlag(&motion, MakeMotionPointer(actor), 1))
        return 0;
    SceneActorChase_SetActorFlags(actor, 0x100);
    actor->base.field2C2 = 64;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    SceneActorTerrain_EnterState(actor, 3, 1, 0);
    SceneActorTerrain_InstallControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorChase_SetActorFlags(actor, 0x10);
    SaveDelay((u8 *)&group->saved, actor->base.onStanding, 9);
    Chase_SnapToRoute(group->destination, &group->homeRoute);
    group->destination->pad = 1;
    actor->base.field2C2 = 64;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    {
        u16 *field = &actor->base.motionFlags;
        u32 mask = ~0x380;
        *field &= mask;
    }
    ClearTargetFlags(motion, ~0x18);
    {
        struct SceneActorRegistryControl *control = &actor->group.common.control;
        if (ActorStatus_BehaviorFlags(actor, 0x200000)) {
            {
                u32 *flags = &actor->base.statusFlags;
                u32 mask = ~0x1000000;
                *flags &= mask;
            }
            SceneActorChase_ClearBits(&actor->base.motionFlags, 0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
    }
    {
        u16 *field = &actor->base.motionFlags;
        u32 mask = ~0x400;
        *field &= mask;
    }
    actor->group.turn.turnsRemaining = 0xff;
    {
        u16 *field = &actor->base.motionFlags;
        u32 mask = ~0x1000;
        *field &= mask;
    }
    return 1;
}
