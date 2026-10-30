#include "global.h"
#define SCENE_ACTOR_CHASE_TYPED_GROUP 1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "display_sequence.h"

static inline u16 *Bat_PrepareLanding(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct DisplaySequenceState *animation;
    u16 *reactionFlags;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    SceneActorChase_SetReactionFlags(actor, 0x10);
    reactionFlags = &actor->flags2D6;
    if (frame <= 17) {
        animation = ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
        if (animation->index == 0 && animation->elapsed == (animation->duration != 0) &&
            animation->duration != 1)
            animation->elapsed = 0;
    }
    return reactionFlags;
}

/* Resume the suspended landing reaction, then queue pursuit or the home route. */
void sub_08087018(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u16 *reactionFlags;
    reactionFlags = Bat_PrepareLanding(actor, frame);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    if (SceneActorChase_QueryMotionFlagComponent(actor, 2)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, group->contactControl, 7);
        SceneActorChase_ClearBits(reactionFlags, 0x983);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
        return;
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 60);
    actor->field2C2 = 64;
    SceneActorChase_SetActorFlags(actor, 0x10);
    SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position, group->destination)) {
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
        SceneActorChase_SetState(actor, 3, 0, 0);
        return;
    }
    group->destination->pad = 1;
}
