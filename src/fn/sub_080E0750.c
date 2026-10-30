#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH           1
#define SCENE_ACTOR_CHASE_FRACTION_ACCESSOR        0
#define SCENE_ACTOR_CHASE_DIRECT_ROUTE_FIELDS      0
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL     1
#define SCENE_ACTOR_CHASE_SCALAR_COORDINATE_OUTPUT 1
#define SCENE_ACTOR_CHASE_ROUTE_COUNT_ACCESSOR     1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

static inline s32 Dog_ReadSecondaryX(const struct SceneActorRegistryResetView *a)
{
    return a->secondaryMotion.x;
}
static inline s32 Dog_ReadSecondaryZ(const struct SceneActorRegistryResetView *a)
{
    return a->secondaryMotion.z;
}

static inline s32 Dog_ReadDesiredX(const struct SceneActorRegistryResetView *a)
{
    return a->reactionStart.x;
}
static inline s32 Dog_ReadDesiredZ(const struct SceneActorRegistryResetView *a)
{
    return a->reactionStart.z;
}
static inline s32 Dog_ScaleMagnitude(s32 value, u32 shift)
{
    return value >> shift;
}
static inline s32 Dog_QuarterDisplacement(const s32 *value)
{
    if (*value >= 0)
        return Dog_ScaleMagnitude(*value, 2);
    return -Dog_ScaleMagnitude(-*value, 2);
}

static inline u8 Dog_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline s32 Dog_ReadDisplacementAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return Dog_QuarterDisplacement(&difference);
}

static inline s32 Dog_ClampAxis(s32 value, s32 limit)
{
    if (value > limit)
        return limit;
    if (value < -limit)
        return -limit;
    return value;
}
/* Ease the contact displacement before resuming pursuit or the home route. */
void sub_080E0750(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *flags;
        u32 mask;
        {
            s32 value = Dog_ReadSecondaryX(actor);
            actor->secondaryMotion.x = Dog_QuarterDisplacement(&value);
        }
        {
            s32 value = Dog_ReadSecondaryZ(actor);
            actor->secondaryMotion.z = Dog_QuarterDisplacement(&value);
        }
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 3);
        mask = 0x20;
        flags = &actor->flags2D4;
        mask |= *flags;
        mask &= ~0x100;
        *flags = mask;
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    dx = Dog_ReadDisplacementAxis(Dog_ReadDesiredX(actor), &actor->reactionEnd.x, &x);
    dz = Dog_ReadDisplacementAxis(Dog_ReadDesiredZ(actor), &actor->reactionEnd.z, &z);
    dx = Dog_ClampAxis(dx, 200);
    dz = Dog_ClampAxis(dz, 200);
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (!Dog_DisplacementStopped(dx, dz))
        goto moving;
    if (!actor->field2B3)
        goto moving;
    group = &actor->group;
    actor->field2B0 = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
    if (actor->field2DC & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return;
    }
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!SceneActorChase_Arrive(actor)) {
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
    SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->field2C2 = 16;
    return;
moving:
    SceneActorChase_SetActorFlags(actor, 0x2000);
}
