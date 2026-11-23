#include "scene_actor_drawable_flags.h"
#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_reaction.h"
#include "runtime.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_damage_flash.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
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
    SceneActorDamage_ClearHitReaction(actor);
    if (actor->base.health > 0)
        return;
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    actor->base.colliderA.responseValue = 0;
    ActorStatus_ClearDrawableFlags(actor, 1);
    {
        void *control = actor->base.onDefeat;
        SceneActorTerrain_InstallControl(actor, control, 17);
    }
    SceneActorTerrain_EnterState(actor, 3, 0, 0);
    actor->base.hitOwners = 32;
    if (!SceneActorChase_Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
}
