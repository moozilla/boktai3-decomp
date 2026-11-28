#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"
#include "scene/motion/scene_actor_charge_offset.h"

void sub_08022790(void *, s32);
s32 Script_Call(u32, void *);
void SceneActorAnimation_UpdateDirectionalCallback(void *);

/* Update the reaction sway and restore the directional state when it completes. */
void Count_UpdateSwayReaction(struct StateActor *base, int time)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)base;
    struct SceneActorBody *body = &actor->body;
    s16 *minimum;
    s16 *maximum;
    u16 old;
    s32 current;

    if (base->actionChanged) {
        int sequence;
        base->actionChanged = 0;
        SceneActors_SetFacing(base, actor->hitAngle + 0x80);
        if (actor->graphics.animationFlags & 1)
            sequence = 11;
        else
            sequence = 49;
        SceneActors_StartDirectionalAnimation(actor, &actor->animation, &actor->animationResource,
                                              sequence, 2, 1, 4);
        if (actor->specialReaction)
            Sound_PlaySongChecked(0x34E);
        Sound_PlaySongChecked(0x34F);
    }
    if (time <= 15)
        SceneActor_SetChargeOffset(actor->hitAngle, &body->motion, 16 - time);
    old = actor->graphics.scaleMin;
    current = actor->graphics.scaleMin;
    minimum = &actor->graphics.scaleMin;
    if (current > 23)
        *minimum = old - 24;
    else
        *minimum = 0;
    old = actor->graphics.scaleMax;
    current = actor->graphics.scaleMax;
    maximum = &actor->graphics.scaleMax;
    if (current > 23)
        *maximum = old - 24;
    else
        *maximum = 0;
    actor->graphics.reactionDelay = 8;
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(base, 12);
    if (actor->specialReaction && !(time & 3))
        sub_08022790(actor, 0);
    if (actor->animation.status & 2) {
        void (*callback)(void *) = SceneActorAnimation_UpdateDirectionalCallback;
        u32 one = 1;
        u32 zero;
        u8 *state = &base->actionState;
        zero = 0;
        *state = one;
        base->actionCallback = (void (*)(void *, int))callback;
        base->callbackFrame = zero;
        base->actionChanged = one;
        SceneActorState_EnterReactionPhase(base, 0);
        if (actor->specialReaction) {
            actor->graphics.animationFlags &= ~1;
            actor->specialReaction = 0;
            if (actor->graphics.reactionScript) {
                Script_Call(actor->graphics.reactionScript, 0);
                actor->graphics.reactionScript = zero;
            }
        }
        {
            u8 variant = zero;
            *minimum = 0;
            *maximum = 0;
            actor->variant = variant;
        }
    } else {
        SceneActorState_SetFlags(base, 1);
    }
}
