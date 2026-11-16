#include "global.h"
#include "dog.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"

struct IvyContext {
    u8 unknown0[0x62C];
    u32 flags;
    u8 unknown630[4];
    u16 field634;
};

struct IvyPlacement {
    u32 xy, z;
};

// confidence=low: clears action gates before resolving the route and stepping its drawable.
void SceneActorMovement_ResolveRoutePosition(void *);
void Dog_ResetActionState(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    context->flags = 0;
    context->timer690 = 0;
    context->destinationCell = 0xFFFF;
    context->timer6A4 = 0;
    context->state6A6 = 0;
    context->emitted = 0;
    Memory_Clear(context->slots, 8);
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
