#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "monsters/scene_actor_chase_step.h"
#include "scene/collision/scene_actor_delayed_response.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorReaction_TryResumeRoute(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.body.direction = (((actor->base.reactionDirection + 3) & 7) << 5) - 64;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
        actor->base.restart = 0;
    }
    if (frame > 60 && actor->base.restart) {
        actor->base.stepDone = 1;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorTerrain_InstallControl(actor, SceneActorControl_OnIdle, 0);
            MonsterInit_SetState(actor, 3, 0, 0);
        } else {
            group->destination->pad = 1;
            MonsterInit_SetState(actor, 3, 1, 0);
            SceneActorTerrain_InstallControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
            MonsterInit_SetBehaviorFlags(actor, 0x10);
            actor->base.field2C2 = 64;
            SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 60);
        }
    }
}
