// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"
struct SlimeAnimationContext {
    u8 unknown0[0x3D4];
    u16 timer;
    u8 unknown3D6[0x34];
    u16 field40A;
};
// confidence=low: clears context timers, selects idle animation, and advances its first frame.
void Slime_ResetIdlePlayback(struct SceneActorRegistryResetView *actor)
{
    struct SlimeAnimationContext *context = actor->context;
    context->field40A = 0;
    context->timer = 0;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    {
        register void *drawable asm("r0") = actor->drawable;
        asm("" : "+r"(drawable));
        DisplaySequence_Step(drawable);
    }
}
