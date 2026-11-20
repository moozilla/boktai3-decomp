#include "global.h"
#include "scene_actor_reaction.h"
#include "runtime.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}
static inline void Reaction_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void Hide(struct ChaseActor *a)
{
    if (!a->base.drawableKind) {
        u32 *flags = a->base.drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneInputWidget *tile = (void *)((u8 *)a->base.drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}
void SceneActorReaction_EndHitCheckDefeat(struct ChaseActor *actor)
{
    {
        u16 *timer = &actor->base.colliderA.responseParameter;
        u32 zero = 0;
        *timer = zero;
        actor->base.hitLockTimer = zero;
    }
    {
        u16 *flags = &actor->base.motionFlags;
        u32 mask = ~0x2000;
        *flags = mask & *flags;
    }
    {
        u16 *flags = &actor->base.reactionFlags;
        u32 mask = ~0x980;
        *flags = mask & *flags;
    }
    if (actor->base.health > 0)
        return;
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    actor->base.colliderA.responseValue = 0;
    Hide(actor);
    {
        void *control = actor->base.onDefeat;
        Reaction_SetControl(actor, 17, control);
    }
    {
        u32 state = 3;
        actor->base.state2A8 = state;
        actor->base.phase2A9 = 0;
        actor->base.behavior2AA = 0;
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
    }
    actor->base.hitOwners = 32;
    if (!Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
}
