// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"

/* The command stepper uses the exact shared route-reset implementation. */

struct SpiderResetContext {
    u8 unknown0[0x628];
    u16 field628, field62A, field62C;
};

void SceneActorMovement_ResolveRoutePosition(void *);
// confidence=low: resets the ghoul route timers, positions, and response state before stepping animation.
extern const u8 gUnk_0824E15C[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void Ghoul_ResetRouteState(struct SceneActorRegistryResetView *actor)
{
    u8 *context = (void *)actor->context;
    *(u16 *)(context + 0x62A) = 0;
    *(u16 *)(context + 0x628) = 0;
    *(u16 *)(context + 0x63C) = 0;
    *(u16 *)(context + 0x648) = 0;
    Memory_Clear(*(void **)((u8 *)actor + 0x5D4), 8);
    Memory_Clear(context + 0x634, 8);
    Memory_Clear(context + 0x62C, 8);
    SceneActorReaction_ResetResponseState((u8 *)actor + 0x460, gUnk_0824E15C, 0x92);
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        register void *drawable asm("r0") = actor->drawable;
        asm("" : "+r"(drawable));
        DisplaySequence_Step(drawable);
    }
}
