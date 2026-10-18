#define SceneActorReaction_ReturnToIdle sub_080EBAE4
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline u8 TakeReset(struct SceneActorRegistryResetView *actor)
{
    u8 *flag = &actor->field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void ResetState(struct SceneActorRegistryResetView *actor)
{
    u32 one = 1;
    u8 *field = &actor->state2A8;
    u32 zero = 0;
    *field = one;
    actor->phase2A9 = zero;
    actor->behavior2AA = zero;
    actor->counter2AC = zero;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = one;
    }
}
void sub_080EBAE4(struct SceneActorRegistryResetView *actor)
{
    TakeReset(actor);
    if (actor->field2B0) {
        ResetState(actor);
        SetControl(actor, sub_0807FC90, 16);
    }
}
