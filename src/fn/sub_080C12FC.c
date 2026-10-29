#include "scene_actor_chase_reaction.h"
#include "scene_actor_animation.h"

struct SkeletonReactionBody {
    u8 unknown0[0x6F0];
    s16 pursuitTimer;
};

void SceneActorScript_TriggerCallOnCount(struct SceneActorRegistryActor *);

/* Resolve contact flags, animation, and pursuit routing. The normal chase
 * continuation returns zero even after installing its control. */
u8 Skeleton_ReactToContact(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonReactionBody *body = actor->context;
    struct SceneActorChaseGroup *group = (struct SceneActorChaseGroup *)&actor->group;

    if (!(actor->field2D8 & 0x20000)) {
        if (actor->reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x13E);
    }
    SceneActorScript_TriggerCallOnCount((struct SceneActorRegistryActor *)actor);
    if (!SceneActorChase_QueryMotionFlags(actor, 0x2000))
        goto otherContact;
    {
        actor->direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
        SceneActorChase_ClearMotionFlags(actor, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(
                    actor, group->route, &actor->position,
                    &((struct SceneActorChaseTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->field2C2 = Div(0x40, 3);
        return 1;
    }
otherContact:
    if (SceneActorChase_QueryMotionFlags(actor, 0x8002)) {
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        SceneActorChase_SetControl(actor, actor->reactionTarget230, 0x12);
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x1000000);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(actor, 0x20);
    if (SceneActorChase_QueryMotionFlags(actor, 0x100))
        return 0;
    if (actor->field2B8)
        return 0;
    if (SceneActorChase_TrackingFlags((struct SceneActorChaseTracking *)&actor->fieldDC, 0x10))
        return 0;
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        if (SceneActorChase_BeginContactAnimation(actor))
            Sound_PlaySong(0x3A7);
        if (!((struct SceneActorChaseTracking *)&actor->fieldDC)->target)
            return 0;
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            body->pursuitTimer = 0;
            if (!SceneActorChase_TryRoute(
                    actor, group->route, &actor->position,
                    &((struct SceneActorChaseTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, group->homeControl, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        actor->field2C2 = 0x20;
        return 1;
    }
    if (SceneActorChase_BeginContactAnimation(actor))
        Sound_PlaySong(0x3A7);
    if (!((struct SceneActorChaseTracking *)&actor->fieldDC)->target)
        return 0;
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!SceneActorChase_Arrive(actor)) {
        body->pursuitTimer = 0;
        if (!SceneActorChase_TryRoute(
                actor, group->route, &actor->position,
                &((struct SceneActorChaseTracking *)&actor->fieldDC)->target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
    }
    SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->field2C2 = 0x20;
    return 0;
}
