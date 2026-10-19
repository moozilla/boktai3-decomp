#define SceneActorReaction_WaitForApproach sub_080ADB90
#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorApproachContext {
    u8 unknown0[0x69C];
    u16 waitTimer;
    u8 unknown69E[0x36];
    u16 flags;
};
void sub_080B1FA8(void *);
static inline u8 Approach_TestFlags(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline void Approach_SetBehavior(struct SceneActorRegistryResetView *actor, u32 state,
                                        u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void sub_080ADB90(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachContext *context = actor->context;
    if (SceneActorReaction_TakePrepared(actor)) {
        context->waitTimer = 0;
    } else {
        u32 mask = 2;
        u16 *flags = &context->flags;
        if (Approach_TestFlags(flags, mask)) {
            if (actor->distanceF0 < 0x19000) {
                sub_080B1FA8(actor);
                SceneActorPlacement_ClearFlags(flags, ~2);
                Approach_SetBehavior(actor, 0, 0, 15);
            } else {
                actor->direction = actor->fieldE9;
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                if (++context->waitTimer > 120) {
                    sub_080B1FA8(actor);
                    SceneActorPlacement_ClearFlags(flags, ~2);
                    Approach_SetBehavior(actor, 0, 0, 15);
                    context->waitTimer = 0;
                }
            }
        } else {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        }
    }
}
