#include "global.h"
#include "scene_actor_reset.h"

struct Golem080CEAC4Context {
    u8 unknown0[0x688];
    u16 field688;
};
struct Golem080CEAC4Body {
    u8 unknown0[9];
    u8 direction;
};
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Golem_UpdateHoldTimer(struct SceneActorRegistryResetView *actor)
{
    struct Golem080CEAC4Context *context = actor->base.context;
    struct Golem080CEAC4Body *body = (struct Golem080CEAC4Body *)((u8 *)actor + 0x50);
    TakeFlag(&actor->base.field2B1);
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    body->direction = actor->base.tracking.heading;
    if ((s32)actor->base.counter2AC > 0x77) {
        SetPhase(actor, 0, 2, 5);
        context->field688 = 0;
    }
    actor->base.counter2AC++;
}
