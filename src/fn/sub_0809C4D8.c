#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"

s32 Mod(s32, s32);
void SceneActorControl_ResetCollisionFlags(struct ChaseActor *);
static inline void SetControl(struct ChaseActor *actor, void *callback, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline void Blink(struct ChaseActor *actor)
{
    u32 mask = 0x20;
    mask |= actor->base.flags2D6;
    actor->base.flags2D6 = mask;
}
static inline void DisableCollider(struct ContactCollider *collider)
{
    u32 mask = ~4;
    collider->flags &= mask;
}
// confidence=low: flashes at increasing frequency, then resets collision and movement state.
void Lun_UpdateTimedRecovery(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl(actor, SceneActorControl_OnIdle, 16);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 16);
        *(u32 *)(state + 0x3D4) |= 1;
        DisableCollider(&actor->base.colliderB);
        DisableCollider(&actor->base.colliderA);
        {
            u16 *flags = &actor->base.flags2C0;
            u32 mask = ~0x800;
            mask &= *flags;
            *flags = mask;
        }
    }
    if ((s32)actor->base.counter2AC < 40) {
        if (Mod(actor->base.counter2AC, 8) == 0)
            Blink(actor);
    } else if (Mod(actor->base.counter2AC, 3) == 0)
        Blink(actor);
    if ((s32)actor->base.counter2AC > 60) {
        actor->base.field2EA += actor->base.body.position.y - actor->base.groundHeight;
        SceneActorControl_ResetCollisionFlags(actor);
        SetState(actor, 0, 0, 0);
    }
    actor->base.counter2AC++;
}
