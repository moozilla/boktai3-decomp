#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "monsters/scene_actor_chase_step.h"
#include "scene/collision/scene_actor_delayed_response.h"
u32 SceneBody_StartRoute(u8 *body, u32 route, u8 id, u8 parameter, u8 point);
// confidence=low: restarts the saved route after a timed animation and waits before restoring control.
void Mummy_ResumeSavedRouteAfterAnimation(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorBody *body;
    struct SceneActorChaseGroup *group = &actor->group;
    struct DisplaySequenceState *sequence;
    body = &actor->base.body;
    sequence = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorRegistryControl *reaction = &group->common.control;
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x141C0);
        reaction->counter = 0;
        reaction->field14 = 0;
        reaction->timer = 0;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 7);
        actor->base.restart = 0;
    }
    if (frame <= 0x3B) {
        sequence->index = 0;
        sequence->elapsed = 0;
    }
    if (actor->base.restart) {
        actor->base.attachmentTimer = 0;
        actor->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
        SceneActorChase_SetControl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
        SceneActorChase_SetState(actor, 3, 1, 0);
        {
            struct SceneActorRegistryRoute *route = &group->homeRoute;
            SceneBody_StartRoute((u8 *)body, (u32)route, group->homeRoute.routeId,
                                 group->homeRoute.parameterB, group->homeRoute.pointId);
            SceneActorChase_SnapToRoute(group->destination, route);
        }
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, (void *)SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else {
            group->destination->pad = 1;
            SceneActorChase_SetActorFlags(actor, 0x10);
            SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 0x3C);
        }
    }
}
