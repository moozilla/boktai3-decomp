#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH 1
#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

u8 Dog_IsActionTimerExpired(struct ChaseActor *);
s32 sub_080DEF78(struct ChaseActor *, u32);

struct DogReactionFields {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x2C];
    u8 turnDelay;
};

/* The reaction fields overlap the canonical actor's species storage. */
union DogReactionContext {
    struct ChaseActor base;
    struct DogReactionFields reaction;
};

static inline s32 Dog_HeadingOctant(s32 angle)
{
    if (angle >= 0)
        return angle >> 5;
    return -(-angle >> 5);
}

/* React to contact, pursue the target, or select a delayed cardinal turn. */
s32 Dog_ChooseReactionBehavior(struct ChaseActor *a)
{
    union DogReactionContext *context = a->base.context;
    struct SceneActorChaseGroup *group = &a->group;
    if (a->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (SceneActorChase_QueryMotionFlags(a, 0x8002)) {
        SceneActorChase_ClearMotionFlags(a, 0x20);
        return 0;
    }
    if (a->base.field2DC & 0xC0000) {
        SceneActorChase_SetControl(a, a->base.reactionTarget230, 18);
        SceneActorChase_SetState(a, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&a->base.flags2D0, 0x1000000);
        SceneActorChase_ClearWordFlags(&context->reaction.flags, 8);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(a, 0x20);
    if (SceneActorChase_QueryMotionFlags(a, 0x100) || a->base.field2B8)
        return 0;
    if (SceneActorChase_QueryMotionFlags(a, 0x80)) {
        SceneActorChase_BeginContactAnimation(a);
        if (!a->base.tracking.target)
            return 0;
        SceneActorChase_SetActorFlags(a, 0x80);
        SceneActorChase_SetFlagComponent((void *)&group->common.gate.flags, 4);
        SceneActorChase_TrackPlayer(a);
        if (Dog_IsActionTimerExpired(a)) {
            if (!SceneActorChase_Arrive(a)) {
                if (!SceneActorChase_TryRoute(a, group->route, &a->base.body.position,
                                              &a->base.tracking.target->position)) {
                    Callback_InvokeActor(a, group->fallback);
                    return 0;
                }
            }
            SceneActorChase_SetControl(a, group->common.gate.control, 26);
            SceneActorChase_SetBehavior(a, 12);
            return 1;
        }
        if (context->reaction.turnDelay)
            return 1;
        SceneActorChase_SetControl(a, SceneActorControl_OnIdle, 16);
        SceneActorChase_SetBehavior(a, 18);
        {
            s32 angle = Dog_HeadingOctant((a->base.tracking.heading + 0xA0) & 255);
            ((struct RoomActor *)a)->nextDirection = (angle + 3) & 7;
        }
        {
            s32 direction = a->base.body.direction;
            a->base.body.direction = (((direction + 0xA0) & 255) >> 6) << 6;
            context->reaction.turnDelay = 90;
            if ((u8)sub_080DEF78(a, direction))
                return 1;
            return 0;
        }
    } else {
        SceneActorChase_BeginContactAnimation(a);
        if (!a->base.tracking.target)
            return 0;
        SceneActorChase_TrackPlayer(a);
        SceneActorChase_SetActorFlags(a, 16);
        group->destination->pad = 0;
        if (!SceneActorChase_Arrive(a)) {
            if (!SceneActorChase_TryRoute(a, group->route, &a->base.body.position,
                                          &a->base.tracking.target->position)) {
                Callback_InvokeActor(a, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(a, a->base.entryTarget228, 1);
        SceneActorChase_SetBehavior(a, 7);
        a->base.field2C2 = 16;
        return 1;
    }
}
