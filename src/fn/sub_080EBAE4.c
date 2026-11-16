#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline u8 TakeReset(struct SceneActorRegistryResetView *actor)
{
    u8 *flag = &actor->base.field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void ResetState(struct SceneActorRegistryResetView *actor)
{
    u32 one = 1;
    u8 *field = &actor->base.state2A8;
    u32 zero = 0;
    *field = one;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = zero;
    actor->base.counter2AC = zero;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = one;
    }
}
void SceneActorReaction_ReturnToIdle(struct SceneActorRegistryResetView *actor)
{
    TakeReset(actor);
    if (actor->base.field2B0) {
        ResetState(actor);
        SetControl(actor, SceneActorControl_OnIdle, 16);
    }
}
