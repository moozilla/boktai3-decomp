#include "global.h"
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

struct DogActionContext {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x14];
    u16 timer690;
    u8 unknown692[0xE];
    u16 selection;
    u8 unknown6A2[2];
    u16 timer6A4;
    u8 state6A6, state6A7;
    u8 unknown6A8[0x30];
    u8 slots[8];
};
// confidence=low: clears action gates before resolving the route and stepping its drawable.
void SceneActorMovement_ResolveRoutePosition(void *);
void Dog_ResetActionState(struct SceneActorRegistryResetView *actor)
{
    struct DogActionContext *context = actor->context;
    context->flags = 0;
    context->timer690 = 0;
    context->selection = 0xFFFF;
    context->timer6A4 = 0;
    context->state6A6 = 0;
    context->state6A7 = 0;
    Memory_Clear(context->slots, 8);
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
