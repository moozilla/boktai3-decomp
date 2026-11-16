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

/* Ivy drawable reset (tentative): clears the context's capture flags, moves the actor by 0x20
 * on x and z, copies the position to the drawable, selects directional entry 0 and steps the
 * display sequence (the step is ScenePanelRig_TickSprites's, with the same pins). The drawable
 * pointer passes through r0, as in the ROM. */
void Ivy_TickDrawable(struct ChaseActor *actor)
{
    {
        struct IvyContext *context = actor->base.context;
        context->flags = 0;
        context->field634 = 0;
    }
    actor->base.body.position.x += 0x20;
    actor->base.body.position.z += 0x20;
    if (!actor->base.drawableKind)
        *(struct IvyPlacement *)((u8 *)actor->base.drawable + 0x1C) =
            *(struct IvyPlacement *)&actor->base.body.position;
    else
        *(struct IvyPlacement *)((u8 *)actor->base.drawable + 0x40) =
            *(struct IvyPlacement *)&actor->base.body.position;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
