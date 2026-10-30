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
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Golem_UpdateHoldTimer(struct SceneActorRegistryResetView *actor)
{
    struct Golem080CEAC4Context *context = actor->context;
    struct Golem080CEAC4Body *body = (struct Golem080CEAC4Body *)((u8 *)actor + 0x50);
    TakeFlag(&actor->field2B1);
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    body->direction = actor->tracking.heading;
    if ((s32)actor->counter2AC > 0x77) {
        SetPhase(actor, 0, 2, 5);
        context->field688 = 0;
    }
    actor->counter2AC++;
}
