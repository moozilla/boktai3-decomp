#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void Vorn_SendMessage(void *);
u8 Vorn_HasActiveResponse(void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);

static inline s32 Vorn_QuarterDisplacement(s32 value)
{
    return value >= 0 ? value >> 2 : -(-value >> 2);
}
static inline s32 Vorn_ReadDisplacementAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return Vorn_QuarterDisplacement(difference);
}
static inline u8 Vorn_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline s32 Vorn_DivideDirection(s32 n)
{
    return n >= 0 ? n >> 5 : -((-n) >> 5);
}
static inline u32 Vorn_StatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u32 Vorn_ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline void Vorn_ClearStatusFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D0 &= mask;
}
static inline void Vorn_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 = mask | actor->flags2D0;
}
static inline void Vorn_ClearTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = ~bits;
    tracking->flags &= mask;
}
static inline void Vorn_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags = bits | tracking->flags;
}
static inline void Vorn_AdjustMotionFlags(struct SceneActorChaseFlags16 *field, u16 set, u32 clear)
{
    u16 value = set;
    u32 result;
    value |= field->value;
    result = value;
    result &= ~clear;
    field->value = result;
}
static inline void Vorn_SetPaletteTimer(struct SceneActorRegistryResetView *actor, u32 value)
{
    actor->field1EA = value;
}
static inline void Vorn_ClearRoute(struct SceneActorRegistryResetView *actor,
                                   struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearActorFlags(actor, 0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}
static inline void Vorn_StartRoute(struct SceneActorRegistryResetView *actor,
                                   struct SceneActorChaseGroup *group)
{
    SceneBody_StartRoute(&actor->body.id, &group->homeRoute, group->homeRoute.routeId,
                         group->homeRoute.parameterB, group->homeRoute.pointId);
}
static inline u8 Vorn_ActorCollisionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    return SceneActorChase_TestField(&actor->flags2C0, bits);
}
static inline void Vorn_ClearContextFlags(u16 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}

void Vorn_UpdateKnockbackRecovery(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    s32 timer;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorChaseFlags16 *motion;
        actor->body.direction = (((actor->body.direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 6);
        if (!Vorn_StatusFlags(actor, 2)) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0, 25);
            Vorn_SetStatusFlags(actor, 2);
            Sound_PlaySong(0x1CC);
        }
        motion = (struct SceneActorChaseFlags16 *)&actor->flags2D4;
        Vorn_AdjustMotionFlags(motion, 2, 0x17A9);
        {
            struct SceneActorRegistryControl *control;
            actor->field2C2 = 64;
            actor->group.common.gate.flags = 0;
            actor->group.common.gate.counter = 0;
            SceneActorChase_ClearFlagComponent(motion, 0x380);
            Vorn_ClearTrackingFlags(&actor->tracking, 0x18);
            control = &actor->group.common.control;
            if (Vorn_ActorFlags(actor, 0x200000)) {
                Vorn_ClearStatusFlags(actor, 0x1000000);
                SceneActorChase_ClearFlagComponent(motion, 0x200);
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
            SceneActorChase_ClearFlagComponent(motion, 0x400);
            actor->group.turn.turnsRemaining = -1;
        }
        SceneActorChase_ClearActorFlags(actor, 0x80);
        actor->field2C2 = 64;
        Vorn_ClearTrackingFlags(&actor->tracking, 0x18);
        Vorn_ClearStatusFlags(actor, 0x1000000);
        Vorn_ClearRoute(actor, &group->common.control);
        SceneActorChase_SetActorFlags(actor, 0x2000000);
        Vorn_SendMessage(actor);
    }
    Vorn_SetPaletteTimer(actor, 65);
    timer = (s16)actor->fieldAA;
    if ((u32)(timer - 30) <= 89) {
        s32 frame = Mod(timer, 30);
        s32 threshold = 15 - Div(timer, 10);
        if (frame < threshold)
            SceneActorChase_SetReactionFlags(actor, 32);
    }
    dx = Vorn_ReadDisplacementAxis(actor->reactionStart.x, &actor->reactionEnd.x, &x);
    dz = Vorn_ReadDisplacementAxis(actor->reactionStart.z, &actor->reactionEnd.z, &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (Vorn_DisplacementStopped(dx, dz) && timer <= 0) {
        struct ContactCollider *collider;
        actor->fieldAA = 0;
        actor->field2B6 = (Vorn_DivideDirection(actor->body.direction) + 7) & 7;
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, group->field5E0, 8);
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 2);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x3A);
        SceneActorChase_SetState(actor, 3, 1, 0);
        actor->body.direction = ((actor->body.direction + 32) >> 6) << 6;
        Vorn_StartRoute(actor, group);
        collider = &actor->colliderA;
        if (Vorn_ActorCollisionFlags(actor, 64))
            collider->flags &= 0xFEFF;
        else if (SceneActorChase_QueryMotionFlagComponent(actor, 0x2016))
            collider->flags &= 0xFEFF;
        else
            collider->flags |= 0x100;
        Vorn_ClearContextFlags(&((struct SceneActorAnimationLink *)actor->context)->flags, 0x1E);
        if (Vorn_HasActiveResponse(actor->context)) {
            Vorn_SetTrackingFlags(&actor->tracking, 8);
            SceneActorChase_SetActorFlags(actor, 0x80);
        }
    }
}
