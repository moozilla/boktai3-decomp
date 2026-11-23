#include "scene_actors.h"
#include "scene_graphics_actor.h"
#include "scene_actor_charge_offset.h"

void sub_08022790(void *, s32);
s32 Script_Call(u32, void *);
void SceneActorAnimation_UpdateDirectionalCallback(void *);

static inline int f08026FF0_choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}

/* Update the reaction sway and restore the directional state when it completes. */
void Vampire_UpdateSwayReaction(struct StateActor *base, int time)
{
    struct StateActor *p = base;
    u8 *position = (u8 *)p + 0x48;
    struct ContactPosition *velocity;
    s16 *minimum;
    s16 *maximum;

    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        SceneActors_SetFacing(p, ((struct SceneGraphicsActor *)p)->hitAngle + 0x80);
        sequence =
            f08026FF0_choose(((struct SceneGraphicsActor *)p)->graphics.animationFlags, 49, 11);
        SceneActors_StartDirectionalAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 2, 1, 4);
        if (((struct SceneGraphicsActor *)p)->specialReaction)
            Sound_PlaySongChecked(0x34E);
        Sound_PlaySongChecked(0x34F);
    }
    if (time <= 15) {
        u32 angle;
        int speed;
        u8 *angleField = &((struct SceneGraphicsActor *)p)->hitAngle;
        velocity = (struct ContactPosition *)(position + 20);
        angle = *angleField;
        speed = 16 - time;
        SceneActor_SetChargeOffset(angle, velocity, speed);
    }
    {
        s16 *value = &((struct SceneGraphicsActor *)p)->graphics.scaleMin;
        u32 old = (u16)*value;
        s32 current = *value;
        minimum = value;
        if (current > 23)
            *minimum = old - 24;
        else
            *minimum = 0;
    }
    {
        s16 *value = &((struct SceneGraphicsActor *)p)->graphics.scaleMax;
        u32 old = (u16)*value;
        s32 current = *value;
        maximum = value;
        if (current > 23)
            *maximum = old - 24;
        else
            *maximum = 0;
    }
    ((struct SceneGraphicsActor *)p)->graphics.reactionDelay = 8;
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(base, 12);
    if (((struct SceneGraphicsActor *)p)->specialReaction && (time & 3) == 0)
        sub_08022790(p, 0);
    if (((struct SceneGraphicsActor *)p)->animation.status & 2) {
        void (*callback)(void *) = SceneActorAnimation_UpdateDirectionalCallback;
        u32 one = 1;
        u32 stateZero;
        u8 *active = &base->actionState;
        stateZero = 0;
        *active = one;
        base->actionCallback = (void (*)(void *, int))callback;
        base->callbackFrame = stateZero;
        base->actionChanged = one;
        SceneActorState_EnterReactionPhase(base, 0);
        if (((struct SceneGraphicsActor *)p)->specialReaction) {
            ((struct SceneGraphicsActor *)p)->graphics.animationFlags &= ~1;
            ((struct SceneGraphicsActor *)p)->specialReaction = 0;
            if (((struct SceneGraphicsActor *)p)->graphics.reactionScript) {
                Script_Call(((struct SceneGraphicsActor *)p)->graphics.reactionScript, 0);
                ((struct SceneGraphicsActor *)p)->graphics.reactionScript = stateZero;
            }
        }
        {
            u8 variant = stateZero;
            *minimum = 0;
            *maximum = 0;
            ((struct SceneGraphicsActor *)p)->variant = variant;
        }
    } else {
        SceneActorState_SetFlags(base, 1);
    }
}
