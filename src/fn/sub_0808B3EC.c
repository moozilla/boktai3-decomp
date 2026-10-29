#define SCENE_ACTOR_CHASE_SCALAR_CELLS 1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_animation.h"
#include "scene_actor_route.h"

void Bat_TickDelayedScript(u8 *);

/* Resolve contact flags, the hurt animation, and pursuit recovery. */
u8 Ghoul_ReactToContact(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRouteContext *context = actor->context;
    struct SceneActorChaseGroup *group = (struct SceneActorChaseGroup *)&actor->group;
    if (!(actor->field2D8 & 0x20000)) {
        if (actor->reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0xD4);
    }
    Bat_TickDelayedScript((u8 *)actor);
    if (SceneActorChase_QueryMotionFlags(actor, 0x2000)) {
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
        actor->field2C2 = 32;
        return 1;
    }
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
    SceneActorChase_ClearBits((u16 *)&context->flags628, 1);
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        SceneActorChase_BeginContactAnimation(actor);
        if (!((struct SceneActorChaseTracking *)&actor->fieldDC)->target)
            return 0;
        SceneActorChase_TrackPlayer(actor);
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(
                    actor, group->route, &actor->position,
                    &((struct SceneActorChaseTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, group->homeControl, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return 1;
    }
    SceneActorChase_BeginContactAnimation(actor);
    if (!((struct SceneActorChaseTracking *)&actor->fieldDC)->target)
        return 0;
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
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
    actor->field2C2 = Div(64, 3);
    return 1;
}
