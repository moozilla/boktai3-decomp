#include "global.h"
/* SceneActorControl_UpdateMotionPhase_080EBD0C: same code as SceneActorControl_UpdateMotionPhase at another address (ported by relatives.py; provisional name) */
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

/* Ivy drawable reset (tentative): clears the context's capture flags, moves the actor by 0x20
 * on x and z, copies the position to the drawable, selects directional entry 0 and steps the
 * display sequence (the step is ScenePanelRig_TickSprites's, with the same pins). The drawable
 * pointer passes through r0, as in the ROM. */
void SceneActorControl_UpdateMotionPhase_080EBD0C(struct SceneActorRegistryResetView *actor)
{
    {
        struct IvyContext *context = actor->context;
        context->flags = 0;
        context->field634 = 0;
    }
    actor->body.position.x += 0x20;
    actor->body.position.z += 0x20;
    if (!actor->drawableKind)
        *(struct IvyPlacement *)((u8 *)actor->drawable + 0x1C) =
            *(struct IvyPlacement *)&actor->body.position;
    else
        *(struct IvyPlacement *)((u8 *)actor->drawable + 0x40) =
            *(struct IvyPlacement *)&actor->body.position;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
