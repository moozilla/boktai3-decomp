#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"

s32 Mod(s32, s32);
void SceneActorControl_ResetCollisionFlags(struct SceneActorRegistryResetView *);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Blink(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 0x20;
    mask |= actor->flags2D6;
    actor->flags2D6 = mask;
}
static inline void DisableCollider(struct ContactCollider *collider)
{
    u32 mask = ~4;
    collider->flags &= mask;
}
// confidence=low: flashes at increasing frequency, then resets collision and movement state.
void Lun_UpdateTimedRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SetControl(actor, SceneActorControl_OnIdle, 16);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 16);
        *(u32 *)(state + 0x3D4) |= 1;
        DisableCollider(&actor->colliderB);
        DisableCollider(&actor->colliderA);
        {
            u16 *flags = &actor->flags2C0;
            u32 mask = ~0x800;
            mask &= *flags;
            *flags = mask;
        }
    }
    if ((s32)actor->counter2AC < 40) {
        if (Mod(actor->counter2AC, 8) == 0)
            Blink(actor);
    } else if (Mod(actor->counter2AC, 3) == 0)
        Blink(actor);
    if ((s32)actor->counter2AC > 60) {
        actor->field2EA += actor->body.position.y - actor->groundHeight2BE;
        SceneActorControl_ResetCollisionFlags(actor);
        SetState(actor, 0, 0, 0);
    }
    actor->counter2AC++;
}
