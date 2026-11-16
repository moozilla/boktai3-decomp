#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "display_sequence.h"

/* This class's own copies of the shared chase inlines it compiles differently. */
static inline s32 Mummy_CellAt(struct ContactPosition *position)
{
    return SceneActorChase_CellIndex(position->x >> 8, position->z >> 8);
}

static inline u32 Mummy_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(Mummy_CellAt(position), position);
}

static inline void Mummy_SnapToRoute(struct ContactPosition *position,
                                     struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Mummy_Height(position);
}

static inline struct SceneActorChaseFlags16 *Mummy_BeginRecovery(struct ChaseActor *actor, u16 bits)
{
    struct SceneActorChaseFlags16 *flags = (void *)&actor->base.reactionFlags;
    SceneActorChase_SetFlagComponent(flags, bits);
    return flags;
}

void Mummy_ResumeRouteAfterRecovery(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *sequence =
        (void *)((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    struct SceneActorChaseFlags16 *reactionFlags;
    SceneActorReaction_TakePrepared(actor);
    reactionFlags = Mummy_BeginRecovery(actor, 0x10);
    if ((((frame <= 0x11) && (!sequence->index)) &&
         (sequence->elapsed == (sequence->duration != 0))) &&
        (sequence->duration != 1))
        sequence->elapsed = 0;
    if (actor->base.restart) {
        struct SceneActorChaseGroup *group = &actor->group;
        actor->base.stepDone = 1;
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
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 0x28);
        {
            u32 mask = 0x10;
            actor->base.behaviorFlags |= mask;
        }
        Mummy_SnapToRoute(actor->group.destination, &actor->group.homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, (void *)SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
}