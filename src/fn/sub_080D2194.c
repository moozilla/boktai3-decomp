#include "global.h"
#define SCENE_ACTOR_CHASE_SCALAR_CELLS 1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "display_sequence.h"

static inline struct SceneActorChaseFlags16 *
Mummy_BeginRecovery(struct SceneActorRegistryResetView *actor, u16 bits)
{
    struct SceneActorChaseFlags16 *flags = (void *)&actor->flags2D6;
    SceneActorChase_SetFlagComponent(flags, bits);
    return flags;
}

void Mummy_ResumeRouteAfterRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct DisplaySequenceState *sequence =
        (void *)((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
    struct SceneActorChaseFlags16 *reactionFlags;
    SceneActorReaction_TakePrepared(actor);
    reactionFlags = Mummy_BeginRecovery(actor, 0x10);
    if ((((frame <= 0x11) && (!sequence->index)) &&
         (sequence->elapsed == (sequence->duration != 0))) &&
        (sequence->duration != 1))
        sequence->elapsed = 0;
    if (actor->field2B3) {
        struct SceneActorChaseGroup *group = &actor->group;
        actor->field2B0 = 1;
        if (SceneActorChase_QueryMotionFlags(actor, 2)) {
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, actor->group.contactControl, 7);
            SceneActorChase_ClearFlagComponent(reactionFlags, 0x983);
            return;
        }
        if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            SceneActorChase_SetBehavior(actor, 14);
            SceneActorChase_SetControl(actor, group->common.gate.control, 0x25);
            return;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 0x28);
        {
            u32 mask = 0x10;
            actor->flags2CC |= mask;
        }
        SceneActorChase_SnapToRoute(actor->group.destination, &actor->group.homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, (void *)SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
}