// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"

// confidence=low: clears mummy motion counters and restarts the directional animation.
void Mummy_ResetMotionState(void *);
void SceneActorMovement_ResolveRoutePosition(void *);

struct MummyMotionCounters {
    u8 unknown0[0x6D0];
    u16 counter6D0, counter6D2, counter6D4, counter6D6;
    u8 unknown6D8[0x1A];
    u16 counter6F2;
};

void Mummy_ResetRevivalMotion(struct SceneActorRegistryResetView *actor)
{
    struct MummyMotionCounters *body = actor->context;
    body->counter6D0 = 0;
    body->counter6D2 = 0;
    body->counter6D4 = 0;
    body->counter6D6 = 0;
    body->counter6F2 = 0;
    actor->group.field5C4 = 0;
    *(u16 *)((u8 *)body + 0x6EE) = 0;
    Mummy_ResetMotionState(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorMovement_ResolveRoutePosition(actor);
    {
        register void *drawable asm("r0") = actor->drawable;
        asm("" : "+r"(drawable));
        DisplaySequence_Step(drawable);
    }
}
